# CANopen
CANopen slave library

## Tasks
### PDO
- PDO mapping sub0: SAM-MDPO and DAM-MPDO
- RPDO
- mappable objects/entries

### SDO
- SDO block transfer

### EMCY
- manufacturer error code
- 1003
- 1029

### TIME
- 

### Other
- save and load, objects 1010 and 1011
- remove #include <cstdio> <cstdlib>
- node.cpp: make sure function code is supported (filters)
- implement all predefined objects
- OD object value range and all other properties (p.91): PDO mapping, range
- remove const access type and only keep ro ?
- create predefined frames inheriting from base Frame ?
- group bits together for access type, PDO mapping, and others ?
- node guarding ?
- SYNC overflow ?

## Tasks (OD generator)
- CLI
- make sure objects are ordered (sort by index)
- perform strict parsing for mandatory or standard entries (like PDO)
    1. 0x1400 to 0x15FF: RPDO communication parameter
    2. 0x1600 to 0x17FF: RPDO mapping parameter
    3. 0x1800 to 0x19FF: TPDO communication parameter
    3. 0x1A00 to 0x1BFF: TPDO mapping parameter
    4. 0x1003: Pre-defined error fields, make sure correct access
- PDO mapping: always allocate 8 entries?
- PDO mapping: enforce correct size
- PDO: enforce PDOs 1 to 4 (standard) ?
- PDO: enforce at least 4 PDOs?

## Remarks
- PDO mapping: I'll do it for bytes, not bits (who maps bits in PDO anyways??)
- Multiple SDOs not supported
- Multiple PDOS above 4 not supported
- Node guarding is not supported, heartbeat only
- VISIBLE_STRING is UTF-8 encoded, OCTET_STRING and UNICODE_STRING are aliases to VISIBLE_STRING
- TIME_OF_DAY and TIME_DIFFERENCE are alisases to UNSIGNED64
- DOMAIN is not supported
- Object 1005 is ignored
- Object 1006 is ignored
