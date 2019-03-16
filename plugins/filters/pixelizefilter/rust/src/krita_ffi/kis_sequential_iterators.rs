use super::{
    kisSequentialConstIteratorNextPixelCallback, kisSequentialConstIteratorOldRawDataCallback,
    kisSequentialIteratorNextPixelCallback, kisSequentialIteratorRawDataCallback,
};

pub trait KisSequentialIteratorBase {
    fn next_pixel(&mut self) -> bool;
}

#[repr(C)]
pub struct KisSequentialConstIterator {
    _private: [u8; 0],
}

impl KisSequentialConstIterator {
    pub unsafe fn old_raw_data(&self, pixel_size: u32) -> &'_ [u8] {
        let old_raw_data_ptr = kisSequentialConstIteratorOldRawDataCallback(self);
        std::slice::from_raw_parts(old_raw_data_ptr, pixel_size as usize)
    }
}

impl KisSequentialIteratorBase for KisSequentialConstIterator {
    fn next_pixel(&mut self) -> bool {
        unsafe { kisSequentialConstIteratorNextPixelCallback(self) }
    }
}

#[repr(C)]
pub struct KisSequentialIterator {
    _private: [u8; 0],
}

impl KisSequentialIterator {
    pub unsafe fn raw_data_mut(&self, pixel_size: u32) -> &'_ mut [u8] {
        let raw_data_ptr = kisSequentialIteratorRawDataCallback(self);
        std::slice::from_raw_parts_mut(raw_data_ptr, pixel_size as usize)
    }
}

impl KisSequentialIteratorBase for KisSequentialIterator {
    fn next_pixel(&mut self) -> bool {
        unsafe { kisSequentialIteratorNextPixelCallback(self) }
    }
}
