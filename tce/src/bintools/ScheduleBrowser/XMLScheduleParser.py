import xml.parsers.expat
import sys
from collections import defaultdict
import itertools

def constant_factory(value):
    return itertools.repeat(value).next

class XMLScheduleParser:

    def __init__(self):
        self.parser_ = xml.parsers.expat.ParserCreate()
        self.parser_.StartElementHandler = self.start_element_handler
        self.parser_.EndElementHandler = self.end_element_handler
        self.parser_.CharacterDataHandler = self.character_data_handler

        self.schedule_ = defaultdict(dict)
        self.incoming_dependences_ = defaultdict(list)
        self.outgoing_dependences_ = defaultdict(list)
        self.labels_ = {}
        self.statistics_ = defaultdict(constant_factory(0))


        # Stack to keep track of element hierarchy
        self.element_stack_ = [('TOP', [])]
        
        self.nodes_ = []
        self.edges_ = []

        self.cvalue_ = ""
        self.cdata_elements_ = ["id", "label", "cycle", "slot", "loop"]

    def parse(self, xml_file_name):
        f = open(xml_file_name)
        self.parser_.ParseFile(f)
        return (self.schedule_,
                self.incoming_dependences_,
                self.outgoing_dependences_,
                self.labels_,
                self.statistics_)

    def start_element_handler(self, name, attributes):
        self.element_stack_.append((name, {}))
        self.cvalue_ = ""

    def end_element_handler(self, name):
        tag, value = self.element_stack_.pop()
        parent_tag, parent_value = self.element_stack_[-1]
        if tag in self.cdata_elements_:
            parent_value[tag] = self.cvalue_
        elif parent_tag == "edge" and tag == "nref":
            if parent_value.has_key("source"):
                parent_value["sink"] = self.cvalue_
            else:
                parent_value["source"] = self.cvalue_
        elif (parent_tag == "edge"
            and tag in ["unknown", "raw", "war", "waw", "trg"]):
            parent_value["type"] = tag
        elif (parent_tag == "edge"
            and tag in ["reg", "mem", "fu", "op", "ra"]):
            parent_value["reason"] = tag
        elif tag == "node":
            self.handle_node(value)
        elif tag == "edge":
            self.handle_edge(value)
        self.cvalue_ = ""

    def character_data_handler(self, data):
        self.cvalue_ += data

    def handle_node(self, node_data):
        if node_data.has_key("cycle"):
            self.schedule_[int(node_data["cycle"])][node_data["slot"]] = node_data["id"]
            self.labels_[node_data["id"]] = node_data["label"]

    def handle_edge(self, edge_data):
        kind = (edge_data["reason"], edge_data["type"])
        if edge_data.has_key("latency"):
            latency = edge_data["latency"]
        else:
            latency = 0
        if edge_data.has_key("loop"):
            loop = True
        else:
            loop = False
        self.incoming_dependences_[edge_data["sink"]].append((edge_data["source"], kind, latency, loop))
        self.outgoing_dependences_[edge_data["source"]].append((edge_data["sink"], kind, latency, loop))
        self.statistics_[kind] += 1
                                

if __name__ == "__main__":
    p = XMLScheduleParser()
    print p.parse(sys.argv[1])
