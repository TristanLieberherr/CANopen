from canopen.objectdictionary import Variable
from .generic import ArrayObject

class Object1011(ArrayObject):
    """Object 1011: Restore default parameters"""
    def __init__(self, index: int, entries: "list[Variable]") -> None:
        super().__init__(index, entries, "Object1011")
