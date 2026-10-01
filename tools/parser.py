def parse_rflags(rflags_val: int) -> dict:
    flags = {
        0: ("CF", "Carry Flag"),
        2: ("PF", "Parity Flag"),
        4: ("AF", "Auxiliary Carry Flag"),
        6: ("ZF", "Zero Flag"),
        7: ("SF", "Sign Flag"),
        8: ("TF", "Trap Flag"),
        9: ("IF", "Interrupt Enable Flag"),
        10: ("DF", "Direction Flag"),
        11: ("OF", "Overflow Flag"),
        12: ("IOPL_L", "I/O Privilege Level (Bit 0)"),
        13: ("IOPL_H", "I/O Privilege Level (Bit 1)"),
        14: ("NT", "Nested Task"),
        16: ("RF", "Resume Flag"),
        17: ("VM", "Virtual-8086 Mode"),
        18: ("AC", "Alignment Check / Access Control"),
        19: ("VIF", "Virtual Interrupt Flag"),
        20: ("VIP", "Virtual Interrupt Pending"),
        21: ("ID", "ID Flag")
    }
    
    parsed = {}
    for bit, (short, long) in flags.items():
        parsed[short] = bool(rflags_val & (1 << bit))
        
    iopl = (rflags_val >> 12) & 0x3
    parsed["IOPL"] = iopl
    return parsed

# Example usage for a typical RFLAGS value (e.g., 0x202)
import sys
if len(sys.argv) > 1:
    val = int(sys.argv[1], 16) if sys.argv[1].startswith('0x') else int(sys.argv[1])
    print(parse_rflags(val))
