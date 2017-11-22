# Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma de
# Barcelona (UAB), and the INTEL Visual Computing Lab.
#
# This work is licensed under the terms of the MIT license.
# For a copy, see <https://opensource.org/licenses/MIT>.

"""CARLA sensors."""


import os


# ==============================================================================
# -- Sensor --------------------------------------------------------------------
# ==============================================================================


class Sensor(object):
    """
    Base class for sensor descriptions. Used to add sensors to the
    CarlaSettings.
    """
    pass


class Camera(Sensor):
    """Camera description. To be added to CarlaSettings."""
    def __init__(self, name, **kwargs):
        self.CameraName = name
        self.PostProcessing = 'SceneFinal'
        self.ImageSizeX = 800
        self.ImageSizeY = 600
        self.CameraFOV = 90
        self.CameraPositionX = 140
        self.CameraPositionY = 0
        self.CameraPositionZ = 140
        self.CameraRotationPitch = 0
        self.CameraRotationRoll = 0
        self.CameraRotationYaw = 0
        self.set(**kwargs)

    def set(self, **kwargs):
        for key, value in kwargs.items():
            if not hasattr(self, key):
                raise ValueError('CarlaSettings.Camera: no key named %r' % key)
            setattr(self, key, value)

    def set_image_size(self, pixels_x, pixels_y):
        self.ImageSizeX = pixels_x
        self.ImageSizeY = pixels_y

    def set_position(self, x, y, z):
        self.CameraPositionX = x
        self.CameraPositionY = y
        self.CameraPositionZ = z

    def set_rotation(self, pitch, roll, yaw):
        self.CameraRotationPitch = pitch
        self.CameraRotationRoll = roll
        self.CameraRotationYaw = yaw


# ==============================================================================
# -- SensorData ----------------------------------------------------------------
# ==============================================================================


class SensorData(object):
    """
    Base class for sensor data returned from the server.
    """
    pass


class Image(SensorData):
    """Data generated by a camera."""
    def __init__(self, width, height, image_type, raw_data):
        assert len(raw_data) == 4 * width * height
        self.width = width
        self.height = height
        self.type = image_type
        self.raw_data = raw_data
        self._converted_data = None

    @property
    def data(self):
        """
        Lazy initialization for data property, stores converted data in its
        default format.
        """
        from . import image_converter

        if self._converted_data is None:
            if self.type == 'Depth':
                self._converted_data = image_converter.depth_to_array(self)
            elif self.type == 'SemanticSegmentation':
                self._converted_data = image_converter.labels_to_array(self)
            else:
                self._converted_data = image_converter.to_rgb_array(self)

        return self._converted_data

    def save_to_disk(self, filename):
        """Save this image to disk (requires PIL installed)."""
        try:
            from PIL import Image as PImage
        except ImportError:
            raise RuntimeError('cannot import PIL, make sure pillow package is installed')

        image = PImage.frombytes(
            mode='RGBA',
            size=(self.width, self.height),
            data=self.raw_data,
            decoder_name='raw')
        b, g, r, a = image.split()
        image = PImage.merge("RGB", (r, g, b))

        folder = os.path.dirname(filename)
        if not os.path.isdir(folder):
            os.makedirs(folder)
        image.save(filename)
