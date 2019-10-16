from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python import pywrap_tensorflow as c_api
from tensorflow.python.util.tf_export import tf_export

from tensorflow.python.ptre.optimizer import ModelAverageOptimizer
from tensorflow.python.ptre.server_lib import PtreServer
from tensorflow.python.ptre.callbacks import InitTrainableVariablesCallback

@tf_export("ptre.init")
def init():
  c_api.PTRE_Init()
