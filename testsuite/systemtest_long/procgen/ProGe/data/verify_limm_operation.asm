CODE;

1 -> integer1.0,               ... [i1.0=42];
 ... ,                         i1.0 -> integer1.1 [i1.0=99];
i1.0 -> integer0.0,            ...;
 ... ,                         i1.0 -> integer2.0 [i1.0=13];
i1.0 -> integer0.1,            ...;
integer0.0 -> fu4.o0.add,      ...;
integer0.1 -> fu4.trigger.add, ...;
fu4.r0 -> integer0.2,          ...;
