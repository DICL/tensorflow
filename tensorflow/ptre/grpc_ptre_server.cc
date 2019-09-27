#include "tensorflow/ptre/grpc_ptre_server.h"

namespace tensorflow {

PtreServiceImpl::PtreServiceImpl(const string& worker,
                                 ::grpc::ServerBuilder* builder,
                                 RdmaMgr* rdma_mgr)
    : local_worker_(worker), rdma_mgr_(rdma_mgr) {
  //rdma_mgr_ = new RdmaMgr(local_worker_);
  cq_ = builder->AddCompletionQueue().release();
}

PtreServiceImpl::~PtreServiceImpl() {
  {
    mutex_lock l(shutdown_mu_);
    is_shutdown_ = true;
  }
  delete cq_;
}

//void PtreServiceImpl::GetRemoteAddress(::grpc::ServerContext* context,
//    const GetRemoteAddressRequest& request,
//    GetRemoteAddressResponse* response) {
Status PtreServiceImpl::GetRemoteAddressSync(
    const GetRemoteAddressRequest* request,
    GetRemoteAddressResponse* response) {
  const string remote_host_name = request->host_name();
  RdmaChannel* rc = rdma_mgr_->FindChannel(remote_host_name);
  RdmaAddress ra;
  ra.lid = request->channel().lid();
  ra.qpn = request->channel().qpn();
  ra.psn = request->channel().psn();
  ra.snp = request->channel().snp();
  ra.iid = request->channel().iid();
  rc->SetRemoteAddress(ra, false);
  rc->Connect();
  int i = 0;
  int idx[] = {1, 0};
  std::vector<RdmaMessageBuffer*> mb(rc->message_buffers());
  for (const auto& mr : request->mr()) {
    RdmaMessageBuffer* rb = mb[idx[i]];
    RemoteMR rmr;
    rmr.remote_addr = mr.remote_addr();
    rmr.rkey = mr.rkey();
    rb->SetRemoteMR(rmr, false);
    i++;
  }

  // Setting up response
  response->set_host_name(local_worker_);
  Channel* channel_info = response->mutable_channel();
  channel_info->set_lid(rc->self().lid);
  channel_info->set_qpn(rc->self().qpn);
  channel_info->set_psn(rc->self().psn);
  channel_info->set_snp(rc->self().snp);
  channel_info->set_iid(rc->self().iid);
  for (int i = 0; i < RdmaChannel::kNumMessageBuffers; i++) {
    MemoryRegion* mr = response->add_mr();
    mr->set_remote_addr(reinterpret_cast<uint64>(mb[i]->buffer()));
    mr->set_rkey(mb[i]->self()->rkey);
  }
  return Status::OK();
}

#define ENQUEUE_REQUEST(method, supports_cancel)                             \
  do {                                                                       \
    mutex_lock l(shutdown_mu_);                                              \
    if (!is_shutdown_) {                                                     \
      Call<PtreServiceImpl, grpc::PtreService::AsyncService,               \
           method##Request, method##Response>::                              \
          EnqueueRequest(this, cq_,                               \
                         &grpc::PtreService::AsyncService::Request##method, \
                         &PtreServiceImpl::method##Handler,                 \
                         (supports_cancel));                                 \
    }                                                                        \
  } while (0)

void PtreServiceImpl::HandleRPCsLoop() {
  for (int i = 0; i < 10; ++i) {
    ENQUEUE_REQUEST(GetRemoteAddress, false);
  }

  void* tag;
  bool ok;

  while (cq_->Next(&tag, &ok)) {
    UntypedCall<PtreServiceImpl>::Tag* callback_tag =
        static_cast<UntypedCall<PtreServiceImpl>::Tag*>(tag);
    if (callback_tag) {
      callback_tag->OnCompleted(this, ok);
    } else {
      cq_->Shutdown();
    }
  }
}

void PtreServiceImpl::GetRemoteAddressHandler(
    WorkerCall<GetRemoteAddressRequest, GetRemoteAddressResponse>* call) {
  Status s = GetRemoteAddressSync(&call->request, &call->response);
  call->SendResponse(ToGrpcStatus(s));
  ENQUEUE_REQUEST(GetRemoteAddress, false);
}

}  // namespace tensorflow
