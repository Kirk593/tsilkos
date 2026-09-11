#![no_std]

#[repr(C)]
pub struct NVMeRegisters {
    pub cap: u64,
    pub vs: u32,
    pub intms: u32,
    pub intmc: u32,
    pub cc: u32,
    pub reserved: u32,
    pub csts: u32,
    pub aqa: u32,
    pub asq: u64,
    pub acq: u64,
}