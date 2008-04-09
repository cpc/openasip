import sys
import os
from pyplusplus import module_builder
from pyplusplus.module_builder.call_policies import *
from pyplusplus import function_transformers as FT

include_paths = ['../..',
                 '../../src/base/mach',
                 '../../src/base/program',
                 '../../src/tools',
                 '../../src/base/tpef',
                 '../../src/base/umach',
                 '../../src/base/osal',
                 '../../src/base/Graph',
                 '../../src/applibs/Scheduler',
                 '../../src/applibs/Scheduler/Algorithms',
                 '../../src/applibs/Scheduler/ResourceManager',
                 '../../src/applibs/Scheduler/ResourceModel',
                 '../../src/applibs/Scheduler/Selector',
                 '../../src/applibs/Scheduler/ProgramRepresentations/ProgramDependenceGraph',
                 '../../src/applibs/Scheduler/ProgramRepresentations/DataDependenceGraph',
                 '../../src/applibs/Scheduler/ProgramRepresentations/ControlFlowGraph',
                 '../../scheduler/passes/BasicBlockScheduler',
                 ]

class binding_generator_t(object):
    def __init__(self,
                 module_name = '',
                 call_policies = (),
                 function_call_policies = (),
                 excluded_classes = (),
                 excluded_members = (),
                 excluded_constructors = (),
                 already_exposed = (),
                 headers = [],
                 extra_headers = [],
                 extra_declarations = [],
                 extra_registrations = [],
                 extra_member_registrations = [],
                 held_types = [],
                 ownership_transfers = []):
        self.module_name = module_name
        self.headers = headers
        hh = map(os.path.basename, headers)
        self.module_builder = module_builder.module_builder_t(
            hh
            , gccxml_path=r"/usr/bin/gccxml" 
            , include_paths=include_paths
            , define_symbols=['TCE_PYTHON_BINDINGS'] )
        self.call_policies = call_policies
        self.function_call_policies = function_call_policies
        self.excluded_classes = excluded_classes
        self.excluded_members = excluded_members
        self.excluded_constructors = excluded_constructors
        self.already_exposed = already_exposed
        self.extra_headers = extra_headers
        self.extra_declarations = extra_declarations
        self.extra_member_registrations = extra_member_registrations
        self.extra_registrations = extra_registrations
        self.held_types = held_types
        self.ownership_transfers = ownership_transfers

    def exclude_classes(self):
        for class_name in self.excluded_classes:
            try:
                print "Exclude %s" % class_name
                self.module_builder.class_(class_name).exclude()
            except RuntimeError:
                print "Did not find class %s to exclude" % class_name

    def exclude_members(self):
        for class_name, member_names in self.excluded_members:
            cl = self.module_builder.class_(class_name)
            for member_name in member_names:
                for member in cl.public_members:
                    if member.name == member_name:
                        member.exclude()

    def exclude_constructors(self):
        for class_name, signature in self.excluded_constructors:
            cl = self.module_builder.class_(class_name)
            for ctor in cl.constructors():
                sig = map(lambda a: "%s" % a.type, ctor.arguments)
                print "Compare '%s' and '%s'" % (signature, sig)
                if sig == signature:
                    ctor.exclude()

    def set_member_call_policies(self):
        default_policies = []
        for p in self.call_policies:
            class_name, member_policies = p
            if class_name == '*':
                default_policies = default_policies + member_policies
            else:
                try:
                    cl = self.module_builder.class_(class_name)
                    for member_name, policy in default_policies + member_policies:
                        try:
                            for member in cl.member_functions(member_name):
                                print "Set policy of %s %s to %s" % (class_name, member_name, policy)
                                member.call_policies = policy
                        except RuntimeError:
                            print "Failed to set policy of %s %s to %s" % (class_name, member_name, policy)
                            # member_functions raises RuntimeError for zero matches
                            pass
                except RuntimeError:
                    print "Did not find class %s" % class_name
                    pass

    def set_function_call_policies(self):
        for name, policy in self.function_call_policies:
            self.module_builder.free_function(name).call_policies = policy

    def add_exception_translation(self):
        self.module_builder.add_registration_code(
            "boost::python::register_exception_translator<Exception>(&translate);")
        self.module_builder.add_declaration_code("""
            /**
            * Translate a C++ exception into a Python exception.
            * See http://www.boost.org/libs/python/doc/v2/exception_translator.html
            */
            
            static void translate(Exception const& e) {
            std::ostringstream o;
            std::string exceptionText;
            
            o << e.lineNum();
            exceptionText = e.fileName() + ":" + o.str() + ":" + e.errorMessage();
            PyErr_SetString(PyExc_RuntimeError, exceptionText.c_str());
            }
        """)

    def handle_already_exposed(self):
        for class_name in self.already_exposed:
            self.module_builder.class_(class_name).already_exposed = True

    def set_held_types(self):
        for class_name, held_type in self.held_types:
            self.module_builder.class_(class_name).held_type = held_type

    def handle_ownership_transfers(self):
        for class_name, member_name, arg_names in self.ownership_transfers:
            cl = self.module_builder.class_(class_name)
            m = cl.member_function(member_name)
            for a in arg_names:
                m.add_transformation(FT.transfer_ownership(a))

    def add_extra_members(self):
        for class_name, members in self.extra_member_registrations:
            cl = self.module_builder.class_(class_name)
            for m in members:
                cl.add_registration_code(m)

    def generate(self):
        mb = self.module_builder
        for eh in self.extra_headers:
            mb.add_declaration_code("#include<%s>" % eh)
        for ed in self.extra_declarations:
            mb.add_declaration_code(ed)
        for er in self.extra_registrations:
            mb.add_registration_code(er)
        mb.calldefs().create_with_signature = True
        self.set_member_call_policies()
        self.set_function_call_policies()
        self.add_extra_members()
        self.exclude_classes()
        self.exclude_members()
        self.exclude_constructors()
        self.handle_already_exposed()
        self.add_exception_translation()
        self.set_held_types()
        self.handle_ownership_transfers()
        mb.build_code_creator( module_name=self.module_name )
        mb.write_module('./%s.cc' % self.module_name)

    def generate_dependences(self):
        n = self.module_name
        deps = "%s.cc %s.d : %s.pypp " % (n, n, n)
        for h in self.headers:
            deps = deps + ("%s " % h)
        f = open("%s.d" % n, "w")
        f.write("%s\n" % deps)
        f.close()
        
                 
    def run(self):
        if len(sys.argv) == 1:
            self.generate()
        elif len(sys.argv) == 2 and sys.argv[1] == '--generate-dependences':
            self.generate_dependences()
        else:
            sys.stderr.write("""Usage:
            python file.pypp
            python file.pypp --generate-dependences
            """)
            exit(1)
    
