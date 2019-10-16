from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python import pywrap_tensorflow as c_api

def _check_incoming():
  return c_api.PTRE_CheckIncoming()
