from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python import pywrap_tensorflow
from tensorflow.python.util.tf_export import tf_export

@tf_export("ptre.Server")
class Server(object):
  def __init__(self, server_def, rank):
    self._server = pywrap_tensorflow.PtreServer_New(
        server_def.SerializeToString(), rank)
