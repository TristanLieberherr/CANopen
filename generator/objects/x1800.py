from .generic import ObjectEntry, RecordObject

class TPDOCommunicationObject(RecordObject):
    def __init__(self, index: int, entries: list[ObjectEntry]) -> None:
        super().__init__(index, entries, "TPDOCommunicationObject")

    def verify(self, objects: dict) -> bool:
        retval = True
        if self.entries[0].defaultValue != len(self.entries) - 1:
            print(f"[Error] Entry {self.varName}: invalid value for sub 0")
            retval = False
        return retval
