from canopen import Node, ObjectDictionary
from canopen.objectdictionary import Record, Variable, Array
import jinja2
import logging


EDS_FILENAME = "example.eds"
TEMPLATES_DIR = "templates"
TEMPLATE_FILENAME = "header.jinja"
HEADER_FILENAME = "test.hpp"

datatype2ctype = {
    0x01: "bool",
    0x02: "int8_t",
    0x03: "int16_t",
    0x04: "int32_t",
    0x05: "uint8_t",
    0x06: "uint16_t",
    0x07: "uint32_t",
    0x08: "float",
    # 0x09: None,     # "VISIBLE_STRING",
    # 0x0A: None,     # "OCTET_STRING",
    # 0x0B: None,     # "UNICODE_STRING",
    # 0x0C: None,     # "TIME_OF_DAY",
    # 0x0D: None,     # "TIME_DIFFERENCE",
    # 0x0F: None,     # "DOMAIN"
    0x10: "int32_t",
    0x11: "double",
    0x12: "int64_t",
    0x13: "int64_t",
    0x14: "int64_t",
    0x15: "int64_t",
    0x16: "uint32_t",
    0x18: "uint64_t",
    0x19: "uint64_t",
    0x1A: "uint64_t",
    0x1B: "uint64_t"
}



class OD_Object:
    def __init__(self, accessType: int, dataType: int, parameterName: str, defaultValue) -> None:
        self.accessType = accessType
        self.dataType = dataType
        self.dataTypeStr = datatype2ctype[dataType]
        self.parameterName = parameterName
        self.defaultValue = 0 if defaultValue is None else defaultValue


class OD_VarEntry:
    """
    The VAR entry stores its value in a single place. It has a single object.
    """
    def __init__(self, index: int, accessType: int, dataType: int, parameterName: str, defaultValue) -> None:
        self.index = index
        self.objectType = 0x07
        self.subNumber = 1
        self.accessType = accessType
        self.dataType = dataType
        self.dataTypeStr = datatype2ctype[dataType]
        self.parameterName = parameterName
        self.defaultValue = 0 if defaultValue is None else defaultValue
        self.varName = 'x%X' % index
        self.objName = 'obj_' + self.varName
        
    def render_OD_Data_declaration(self) -> "list[str]":
        return [f"{self.dataTypeStr} {self.varName}"]
    
    def render_OD_Data_constructor(self) -> "list[str]":
        return [f"{self.varName}({self.defaultValue})"]
    
    def render_OD_Object_declaration(self) -> str:
        return f"OD_Object {self.objName}[1]"
    
    def render_OD_Object_constructor(self) -> str:
        return f"{self.objName}{{OD_Object({self.accessType}, {self.dataType}, &data.{self.varName})}}"
    
    def render_OD_Entry_constructor(self) -> str:
        return f"OD_ObjectEntry({self.index}, {self.objectType}, {self.subNumber}, objects.{self.objName})"
    

class OD_ArrayEntry:
    """
    The ARRAY entry stores an array of size subNumber - 1 for its values, plus a single value for sub0. It has subNumber objects.
    """
    def __init__(self, index: int, objects: "list[OD_Object]") -> None:
        self.index = index
        self.objectType = 0x08
        self.dataType = objects[0].dataType
        self.dataTypeStr = datatype2ctype[self.dataType]
        self.subNumber = len(objects)
        self.objects = objects
        self.varName = 'x%X' % index
        self.objName = 'obj_' + self.varName
        self.sub0Name = self.varName + 'sub0'

    def render_OD_Data_declaration(self) -> "list[str]":
        sub0 = f"{datatype2ctype[0x05]} {self.sub0Name}"
        arr = f"{self.dataTypeStr} {self.varName}[{self.subNumber - 1}]"
        return [sub0, arr]
    
    def render_OD_Data_constructor(self) -> "list[str]":
        sub0 = f"{self.sub0Name}({self.subNumber})"
        defVals = ', '.join([str(obj.defaultValue) for obj in self.objects[1:]])
        arr = f"{self.varName}{{{defVals}}}"
        return [sub0, arr]
    
    def render_OD_Object_declaration(self) -> str:
        return f"OD_Object {self.objName}[{self.subNumber}]"
    
    def render_OD_Object_constructor(self) -> str:
        sub0 = f"OD_Object({self.objects[0].accessType}, {self.objects[0].dataType}, &data.{self.sub0Name})"
        arr = ', '.join([f"OD_Object({obj.accessType}, {obj.dataType}, &data.{self.varName}[{i}])" for i, obj in enumerate(self.objects[1:])])
        return f"{self.objName}{{{sub0}, {arr}}}"
    
    def render_OD_Entry_constructor(self) -> str:
        return f"OD_ObjectEntry({self.index}, {self.objectType}, {self.subNumber}, objects.{self.objName})"
    

class OD_RecordEntry:
    """
    The RECORD entry stores a struct of subNumber fields for its values. It has subNumber objects. 
    """
    def __init__(self, index: int, objects: "list[OD_Object]") -> None:
        self.index = index
        self.objectType = 0x09
        self.subNumber = len(objects)
        self.objects = objects
        self.varName = 'x%X' % index
        self.objName = 'obj_' + self.varName
        self.sub0Name = self.varName + 'sub0'

    def render_OD_Data_declaration(self) -> "list[str]":
        fields = '; '.join([f"{obj.dataTypeStr} sub{i}" for i, obj in enumerate(self.objects)]) + ";"
        return [f"struct{{{fields}}} {self.varName}"]
    
    def render_OD_Data_constructor(self) -> "list[str]":
        defVals = ', '.join([str(obj.defaultValue) for obj in self.objects])
        return [f"{self.varName}{{{defVals}}}"]

    def render_OD_Object_declaration(self) -> str:
        return f"OD_Object {self.objName}[{self.subNumber}]"
    
    def render_OD_Object_constructor(self) -> str:
        arr = ', '.join([f"OD_Object({obj.accessType}, {obj.dataType}, &data.{self.varName}.sub{i})" for i, obj in enumerate(self.objects)])
        return f"{self.objName}{{{arr}}}"
    
    def render_OD_Entry_constructor(self) -> str:
        return f"OD_ObjectEntry({self.index}, {self.objectType}, {self.subNumber}, objects.{self.objName})"




def toEntry(entry): # TODO: restore and convert access type
    try:
        if isinstance(entry, Variable):
            return OD_VarEntry(entry.index, 0, entry.data_type, entry.name, entry.default)
        if isinstance(entry, Array):
            objs = [OD_Object(0, obj.data_type, obj.name, obj.default) for obj in list(entry.values())]
            return OD_ArrayEntry(entry.index, objs)
        if isinstance(entry, Record):
            objs = [OD_Object(0, obj.data_type, obj.name, obj.default) for obj in list(entry.values())]
            return OD_RecordEntry(entry.index, objs)
    except KeyError: logging.warning(f"Skipping entry [{'0x%X' % entry.index}]")
    return None
    

od: ObjectDictionary = Node(1, EDS_FILENAME).object_dictionary
objectEntries = [toEntry(entry) for entry in od.values() if toEntry(entry) is not None]
allDataDeclarations = [declaration for sublist in objectEntries for declaration in sublist.render_OD_Data_declaration()]
allDataConstructors = [constructor for sublist in objectEntries for constructor in sublist.render_OD_Data_constructor()]
allObjectDeclarations = [entry.render_OD_Object_declaration() for entry in objectEntries]
allObjectConstructors = [entry.render_OD_Object_constructor() for entry in objectEntries]
allEntryConstructors = [entry.render_OD_Entry_constructor() for entry in objectEntries]
# for entry in objectEntries: print(entry.render_OD_Data_declaration())
# for entry in allObjectConstructors: print(entry)




variables = {
    "od_objects_count": len(od),
    "od_object_entries": objectEntries,
    "od_all_data_declarations": allDataDeclarations,
    "od_all_data_constructors": allDataConstructors,
    "od_all_object_declarations": allObjectDeclarations,
    "od_all_object_constructors": allObjectConstructors,
    "od_all_entry_constructors": allEntryConstructors
}

env = jinja2.Environment(loader=jinja2.FileSystemLoader(TEMPLATES_DIR), trim_blocks=True, lstrip_blocks=True)
env.get_template(TEMPLATE_FILENAME).stream(**variables).dump(HEADER_FILENAME)



# for obj in od.values():
#     if obj.index < 0x1000: continue
#     if isinstance(obj, Record):
#         print(f"{obj.name}:")
#         for var in obj.subindices.values():
#             var: Variable
#             print(f"  {datatype2ctype[var.data_type]} {var.name}: {var.default}")
#         print()

