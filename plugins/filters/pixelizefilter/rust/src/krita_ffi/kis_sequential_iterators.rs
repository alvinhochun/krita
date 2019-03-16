use super::KisPaintDeviceSP;

pub trait KisSequentialIteratorBase {
    fn next_pixel(&mut self) -> bool;
}

#[repr(transparent)]
pub struct KisSequentialConstIterator {
    _private: [u8; 64],
}

impl KisSequentialConstIterator {
    pub fn new(
        src_dev: *mut KisPaintDeviceSP,
        left: i32,
        top: i32,
        width: i32,
        height: i32,
    ) -> Self {
        let mut obj = Self { _private: [0; 64] };
        unsafe {
            ffi_KisSequentialConstIterator__ctor_checked(
                &mut obj,
                std::mem::size_of::<Self>() as isize,
                src_dev,
                left,
                top,
                width,
                height,
            );
        }
        obj
    }

    pub unsafe fn old_raw_data(&self, pixel_size: u32) -> &'_ [u8] {
        let old_raw_data_ptr = ffi_KisSequentialConstIterator_oldRawData(self);
        std::slice::from_raw_parts(old_raw_data_ptr, pixel_size as usize)
    }
}

impl Drop for KisSequentialConstIterator {
    fn drop(&mut self) {
        unsafe {
            ffi_KisSequentialConstIterator__dtor(self);
        }
    }
}

impl KisSequentialIteratorBase for KisSequentialConstIterator {
    fn next_pixel(&mut self) -> bool {
        unsafe { ffi_KisSequentialConstIterator_nextPixel(self) }
    }
}

#[repr(transparent)]
pub struct KisSequentialIterator {
    _private: [u8; 64],
}

impl KisSequentialIterator {
    pub fn new(
        src_dev: *mut KisPaintDeviceSP,
        left: i32,
        top: i32,
        width: i32,
        height: i32,
    ) -> Self {
        let mut obj = Self { _private: [0; 64] };
        unsafe {
            ffi_KisSequentialIterator__ctor_checked(
                &mut obj,
                std::mem::size_of::<Self>() as isize,
                src_dev,
                left,
                top,
                width,
                height,
            );
        }
        obj
    }

    pub unsafe fn raw_data_mut(&self, pixel_size: u32) -> &'_ mut [u8] {
        let raw_data_ptr = ffi_KisSequentialIterator_rawData(self);
        std::slice::from_raw_parts_mut(raw_data_ptr, pixel_size as usize)
    }
}

impl Drop for KisSequentialIterator {
    fn drop(&mut self) {
        unsafe {
            ffi_KisSequentialIterator__dtor(self);
        }
    }
}

impl KisSequentialIteratorBase for KisSequentialIterator {
    fn next_pixel(&mut self) -> bool {
        unsafe { ffi_KisSequentialIterator_nextPixel(self) }
    }
}

extern "C" {
    // -- KisSequentialConstIterator --
    fn ffi_KisSequentialConstIterator__ctor_checked(
        this_ptr: *mut KisSequentialConstIterator,
        this_size: isize,
        src_dev: *mut KisPaintDeviceSP,
        left: i32,
        top: i32,
        width: i32,
        height: i32,
    );
    fn ffi_KisSequentialConstIterator__dtor(this_ptr: *mut KisSequentialConstIterator);
    fn ffi_KisSequentialConstIterator_nextPixel(it: *mut KisSequentialConstIterator) -> bool;
    fn ffi_KisSequentialConstIterator_oldRawData(
        it: *const KisSequentialConstIterator,
    ) -> *const u8;

    // -- KisSequentialIterator --
    fn ffi_KisSequentialIterator__ctor_checked(
        this_ptr: *mut KisSequentialIterator,
        this_size: isize,
        src_dev: *mut KisPaintDeviceSP,
        left: i32,
        top: i32,
        width: i32,
        height: i32,
    );
    fn ffi_KisSequentialIterator__dtor(this_ptr: *mut KisSequentialIterator);
    fn ffi_KisSequentialIterator_nextPixel(it: *mut KisSequentialIterator) -> bool;
    fn ffi_KisSequentialIterator_rawData(it: *const KisSequentialIterator) -> *mut u8;
}
