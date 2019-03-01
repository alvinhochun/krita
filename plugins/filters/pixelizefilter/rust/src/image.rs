use crate::krita_ffi::KisSequentialConstIterator;
use crate::krita_ffi::KisSequentialIterator;
use crate::krita_ffi::KisSequentialIteratorBase;

pub(crate) struct PixelIterator<'a, T: KisSequentialIteratorBase> {
    it: &'a mut T,
    pixel_size: u32,
}

impl<'a, T: KisSequentialIteratorBase> PixelIterator<'a, T> {
    pub(crate) unsafe fn new(it: &'a mut T, pixel_size: u32) -> Self {
        Self { it, pixel_size }
    }

    pub(crate) fn into_inner(self) -> &'a mut T {
        self.it
    }
}

impl<'a> PixelIterator<'a, KisSequentialConstIterator> {
    pub(crate) fn next_old_raw_data(&mut self) -> Option<&'_ [u8]> {
        if self.it.next_pixel() {
            Some(unsafe { self.it.old_raw_data(self.pixel_size) })
        } else {
            None
        }
    }
}

impl<'a> PixelIterator<'a, KisSequentialIterator> {
    pub(crate) fn next_raw_data_mut(&mut self) -> Option<&'_ mut [u8]> {
        if self.it.next_pixel() {
            Some(unsafe { self.it.raw_data_mut(self.pixel_size) })
        } else {
            None
        }
    }
}
