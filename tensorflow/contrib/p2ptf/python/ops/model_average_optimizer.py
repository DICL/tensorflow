from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python.framework import ops
from tensorflow.python.ops import control_flow_ops
from tensorflow.python.ops import math_ops
from tensorflow.python.ops import state_ops

class ModelAverageOptimizer(tf.train.Optimizer):
  def __init__(self,
               opt,
               communication_period=10,
               use_locking=True,
               name='ModelAverageOptimizer'):
    super(ModelAverageOptimizer, self).__init__(use_locking, name)
    self._opt = opt
    self._period = communication_period
    self._push = dddl_push

  def apply_gradients(self, grads_and_vars, global_step=None, name=None):

    def _push_model():
      local_vars = [v for g, v in grads_and_vars if g is not None]
      push_ops = []
      for v in local_vars:
        push_ops.append(
            self._push(v))
      variable_push = control_flow_ops.group(*(push_ops))
      return variable_push

    def _check_incoming():
      return tf.constant(True)

    def _sync_model():
      local_vars = [v for g, v in grads_and_vars if g is not None]
      sync_ops = []
      for v in local_vars:
        sync_ops.append(
            self._sync(v))
      variable_sync = control_flow_ops.group(*(sync_ops))
      return variable_sync

    # STEP1: Local Update
    apply_updates = self._opt.apply_gradients(grads_and_vars)
    with ops.control_dependencies([apply_updates]):
      local_update = state_ops.assign_add(
          self._local_step, 1, name='local_step_update').op
    # STEP2: Sync with Incoming Remote Model
    with ops.control_dependencies([local_update]):
      sync_cond = self._check_incoming()
      remote_sync = control_flow_ops.cond(
          sync_cond, _sync_model, control_flow_ops.no_op)
    # STEP3: Push Model
    with ops.control_dependencies([remote_sync]):
      push_cond = math_ops.equal(
          math_ops.mod(self._local_step, self._period), 0)
      remote_push = control_flow_ops.cond(
          push_cond, _push_model, control_flow_ops.no_op)
    return remote_push 
