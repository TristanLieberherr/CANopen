from canopen.objectdictionary import Variable
from .generic import RecordObject

class Object1A00(RecordObject):
    def __init__(self, index: int, entries: list[Variable]) -> None:
        super().__init__(index, entries, "Object1A00")

    def verify(self, objects: dict) -> bool:
        retval = True
        for i in range(self.entries[0].defaultValue):
            entry = self.entries[i + 1]
            index = entry.defaultValue >> 16 & 0xFFFF
            subindex = entry.defaultValue >> 8 & 0xFF
            length = entry.defaultValue & 0xFF
            if index not in objects:
                self.error(f"invalid TPDO mapping for sub {i + 1} (invalid index)")
                retval = False
            elif objects[index].subNumber <= subindex:
                self.error(f"invalid TPDO mapping for sub {i + 1} (invalid subindex)")
                retval = False
        return retval
