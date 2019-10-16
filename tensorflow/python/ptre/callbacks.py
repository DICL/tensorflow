from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python import pywrap_tensorflow as c_api
from tensorflow.python.ops.variables import Variable
from tensorflow.python.util import compat
from tensorflow.python.keras.callbacks import Callback
from tensorflow.python.util.tf_export import tf_export

@tf_export("ptre.InitTrainableVariablesCallback")
class InitTrainableVariablesCallback(Callback):
  def __init__(self, server):
    super(InitTrainableVariablesCallback, self).__init__()
    self._server = server

  def on_train_begin(self, logs={}):
    print(self.model.trainable_variables)
    var_names = [compat.as_bytes(v.name) for v in self.model.trainable_variables]
    print(var_names)
    tensors = [Variable._TensorConversionFunction(v) for v in self.model.trainable_variables]
    nvars = len(self.model.trainable_variables)
    #for var in self.model.trainable_variables:
      #var_names.append(compat.var.name)
      #tensors.append(Variable._TensorConversionFunction(var))
    c_api.PTRE_InitTrainableVariables(self._server, var_names, tensors, nvars)
