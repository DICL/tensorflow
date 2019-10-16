from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python import pywrap_tensorflow as c_api
from tensorflow.python.util.tf_export import tf_export
# TODO: from tensorflow.python.ptre import VariableArray

@tf_export("ptre.Server")
class PtreServer(object):
  def __init__(self, rank):
    self._rank = rank
    self._server = c_api.PTRE_NewServer(self._rank)

  def __del__(self):
    c_api.PTRE_ServerStop(self._server)

  def check_incoming(self):
    c_api.PTRE_CheckIncoming(self._server)

  def register_variable(self, var):
    """

    Args:
      var: tf.Variable
    """
    try:
      self._variables.append(var)
    except:
      self._variables = []
      self._variables.append(var)

  def init_remote_store(self):
    for var in self._variables:
      #TODO: impl.
      #c_api.PTRE_RegisterVariable(self._server, var)
      continue
    #c_api.PTRE_InitRemoteStore(self._server)

  #def register_trainable_variables(self, trainable_variables):
  #  """

  #  Args:
  #    trainable_variables: List of tensors.
  #  """

  #def trainable_variables(self):
  #  return self._trainable_variables
