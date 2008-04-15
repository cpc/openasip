//
// TCE Instruction formats.
//
// Only one simple format is currently available.
//
// Author: Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
//

class InstTCE<dag outOps, dag inOps, string asmstr,
              list<dag> pattern> : Instruction {

    let Namespace = "TCE";
    dag InOperandList = inOps;
    dag OutOperandList = outOps;
    let AsmString = asmstr;
    let Pattern = pattern;
}

class Pseudo<dag outOps, dag inOps,
             string asmstr, list<dag> pattern>
   : InstTCE<outOps, inOps, asmstr, pattern>;
