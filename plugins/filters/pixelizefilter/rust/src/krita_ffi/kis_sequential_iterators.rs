pub trait KisSequentialIteratorBase {
    fn next_pixel(&mut self) -> bool;
}

#[repr(C)]
pub struct KisSequentialConstIterator {
    _private: [u8; 0],
}

impl KisSequentialConstIterator {
    pub unsafe fn old_raw_data(&self, pixel_size: u32) -> &'_ [u8] {
        let old_raw_data_ptr = ffi_KisSequentialConstIterator_oldRawData(self);
        std::slice::from_raw_parts(old_raw_data_ptr, pixel_size as usize)
    }
}

impl KisSequentialIteratorBase for KisSequentialConstIterator {
    fn next_pixel(&mut self) -> bool {
        unsafe { ffi_KisSequentialConstIterator_nextPixel(self) }
    }
}

#[repr(C)]
pub struct KisSequentialIterator {
    _private: [u8; 0],
}

impl KisSequentialIterator {
    pub unsafe fn raw_data_mut(&self, pixel_size: u32) -> &'_ mut [u8] {
        let raw_data_ptr = ffi_KisSequentialIterator_rawData(self);
        std::slice::from_raw_parts_mut(raw_data_ptr, pixel_size as usize)
    }
}

impl KisSequentialIteratorBase for KisSequentialIterator {
    fn next_pixel(&mut self) -> bool {
        unsafe { ffi_KisSequentialIterator_nextPixel(self) }
    }
}

extern "C" {
    // -- KisSequentialConstIterator --
    fn ffi_KisSequentialConstIterator_nextPixel(it: *mut KisSequentialConstIterator) -> bool;
    fn ffi_KisSequentialConstIterator_oldRawData(
        it: *const KisSequentialConstIterator,
    ) -> *const u8;

    // -- KisSequentialIterator --
    fn ffi_KisSequentialIterator_nextPixel(it: *mut KisSequentialIterator) -> bool;
    fn ffi_KisSequentialIterator_rawData(it: *const KisSequentialIterator) -> *mut u8;
}
