Using binary handling module v6.8.1 -----------------------------------

version history
---------------

v6.0.1	                Initial version.
v6.1.1	                Added guide for handling relocation section
                        and info about how reference manager actually works.
v6.2.1  03.07.2003      Class diagram, sequence diagram and this usage
                        manual reviewed and updated. Added 'design background'
                        and 'about SafePointers' sections. -- poj
v6.2.2  03.07.2003      Added some text to section about reference manager.
v6.5.1  09.07.2003      Changed to match current plan of ReferenceManager and
                        SafePointers.
v6.5.2  29.08.2003      Updated to last changes to design and code.
v6.8.1  01.09.2003      Changed first part of reading to match implementation.
v6.9	26.09.2003	Try the change the reading of a.out to match current
			implementation.
v6.9.1	26.09.2003	Text cleanup, corrected minor errors.

Design Background
-----------------

The binary handling module takes heavily advantage of prototype design
pattern.  You can find information of the pattern from:

http://home.earthlink.net/~huston2/dp/prototype.html

For example code, see especially the last example in:

http://home.earthlink.net/~huston2/dp/PrototypeDemosCpp


Indented code lines in this document are something like pseudo C++.


Reading a Binary File
---------------------

Binary reading is done by asking BinaryReader class to read given stream.
Because of prototype design pattern BinaryReader can be done automatically
determine if it can read given stream by asking every BinaryReader istance
if it can read given binary stream.

        BinaryStream myStream("myfile.bin");

        Binary* bin = BinaryReader::readBinary(myStream);

This causes BinaryReader class to read binary stream with instance of 
BinaryReader that can read given stream. If it doesn't exist BinaryReader 
just throws an InstanceNotFound exception.

In BinaryReader (for example in this case it's concrete class is AOutReader)
a new Binary file is created.

        Binary* newBin = new Binary();

Binary file is read and when new section headers are found, new section
objects are constructed for them.

Let's say AOutReader::ReadBinary() finds a text section.

In a.out there is no id's for sections. Since there are only one section of
each type (text, data, text relocation...) we can use an enumeration. This 
enumeration is placed in AOutSymbolSectionReader and it is used to inform
reference Manager about sections.  

Instance of correct section class is created. 

        Section* sect = Section::CreateSection(Section::ST_TEXT);

We add pointer to this Section to the Binary:

        newBin->AddSection(sect);

If section points to other sections, SafePointers are created for these
references.  There's more information about SafePointers and
ReferenceManager later in this text. 

In a.out text section doesn't point to anywhere else, so link field is 
set to NULL

	sect->setLink(SafePointer::null);

ReferenceManager is also informed of our newly created section to make
reference handling possible. 

	unsigned sect_id = AOutSymbolSectionReader::ST_TEXT;
        ReferenceManager::AddObjectRef(SectionKey(sect_id), sect);

Then the stream and the section to be constructed are given to a correct
SectionReader which is found using FindSectionReader function of
SectionReader.

        SectionReader::FindSectionReader(sect.type())->
            ReadSection(myStream, sect, offset, length);

ReadSection() parameters:
sect    pointer to a Section object to add element to
offset  the offset in binary stream where section's data is found
length  the length of the section's data.

FindSectionReader finds correct SectionReader subclass and returns a pointer
to an instance of it. In this case sectType is ST_TEXT so AOutSectionReader
returns a pointer to a AOutTextSectionReader instance.

AOutTextSectionReader reads section's data from binary stream. Specifically,
it constructs InstructionElements from the binary data it reads from the
binary stream.

Before reading any element, the current section offset is saved to
be given for ReferenceManager to be used as the key for the element to be
read next. Also the index of the element is stored.

	int index = 1;
	// compute offset relative to section start offset
        unsigned int sectionOffset = myStream.pos() - offset;

Let's say a single move is detected and its information is read from the
binary stream.

        Move* newElem = new Move();

In tpef an instruction consist always of one Move or one Immediate and
one Move. So the first flag of Move read is set to true:

    
        newElem->setBegin(true);

Then we read all the possible data of the instruction element from the
stream, for example the source index of the move. This index must be
converted to the exact value of source register:

        newElem->setSrcIndex(convertSource(newElem, myStream->ReadByte()));

We must inform ReferenceManager of this object for reference handling. 
We use the starting offset of this element in the binary stream as a key for
the ReferenceManager.

        SectionOffsetKey key = SectionOffsetKey(sect_id, sectionOffset);
        ReferenceManager::AddObjectRef(key, newElem);

We also know this instruction element's index in the section so we may tell
it to ReferenceManager too so this element can be referenced with this kind
of key also.

        SectionIndexKey key = SectionIndexKey(sect_id, index);
        ReferenceManager::AddObjectRef(key, newElem);

Then we simply add the element we just read to the section's data:

        sect->addElement(newElem);

Elements are read until the stream is in the position (offset + length).

All section elements, in this case of type InstructionElement, are added to
the TextSection and references (in this case section offsets) to
InstructionElement objects are given to the ReferenceManager so it can
handle possible future references to these elements in the binary file and
connect them to correct objects.

Notice that the only place where we knew the real type of SectionElement and
Section was in this concrete SectionReader (AOutTextSectionReader), thanks
to prototype design pattern. This allows adding the ability to read new
types of sections, for example from a.out files, just by linking a new
AOutSectionReader derived class to the main program.


How ReferenceManager is used?
-----------------------------

Now let's pretend that myReader finds a relocation section. Again, we ask
from Section for section instance of section type we found.

        Section* sect = Section::CreateSection(Section::ST_RELOC);

We add this section to the Binary and register this section into
ReferenceManager same way we did for text section in the last example. We
also set data that is common for all sections.

        newBin->AddSection(sect);
	unsigned sect_id = AOutSymbolSectionReader::ST_RELOC;
        ReferenceManager::AddObjectRef(SectionKey(sect_id), sect);

And so on. See also the previous example.

Again, the stream to read from and the section to be constructed are given
to a correct SectionReader.

	SectionReader* sr = AOutSectionReader::FindSectionReader(sect.type());
	sr->ReadSection(myStream, sect, offset, length);

Because in ReadSection method we know which is the concrete section type we
are reading, we also know what type of elements it has. So in this case we
insert relocation elements to the section.

        for (each relocation in myStream from offset to offset+length)
        {
	sectOffset = myStream.pos() - offset;
        RelocEntry *newReloc = RelocEntry();
        SectionOffsetKey key = SectionOffsetKey(sect_id, sectOffset);
        ReferenceMAnager::AddObjectReference(key, newReloc);
        sect->AddElement(newReloc);

The relocation type and relocation reference data are read from the stream.
relocation_type is also converted to match the relocation types of our
object model.  For example like this:

        relocation_type = typeChart[relocation_type];

When reading a.out relocation_type is always RelocSection::RT_RELATIVE, so
in this case there is no need for conversions.

ReferenceManager is asked to give SafePointers to the elements we are
referencing.

        newReloc->setSrcElement(new SafePointer(
            SectionOffsetKey(srcId, srcOffset)));

        newReloc->setDstElement(new SafePointer(
            SectionOffsetKey(dstID, dstoffset)));

        newReloc->setType(relocation_type);
        }

The BinaryReader is responsible for generating unique keys for loaded and
referenced elements and sections. If BinaryReader attempts to register two
objects with the same key, the ReferenceManager will throw an exception.
(The ReferenceManager could tolerate multiple registrations with the same
keys if the value to register is the same that is already registered. In
this case, nothing happens).


How ReferenceManager Handles References?
----------------------------------------

Reference manager does reference solving (connecting SafePointers to correct
objects) using SafePointer objects. With ReferenceManager it's possible to
get references to objects that are not yet created using different types of
keys that identify the object to be created.


The Key Maps
------------

The core data structures of the ReferenceManager are maps, one for each
different kind of key.  Each map entry has a pointer to a list of
SafePointers as value.

For example, if we are using the three different key types (FileOffsetKey, 
SectionOffsetKey and SectionIndexKey) we have three different key maps.

 key            value
 ______________ _____________________________________________
|FileOffsetKey | List<SafePointer*>*                         |
|--------------|---------------------------------------------|
|   (1232212)  | [points to list (c) in reference map]       | (d)
|--------------|---------------------------------------------|
|              |                                             |
|--------------|---------------------------------------------|
|              |                                             |
'--------------'---------------------------------------------'

key                value
 _________________ _______________________________________________
|SectionOffsetKey | List<SafePointer*>*                           |
|-----------------|-----------------------------------------------|
|    (2, 1221)    | {0x2212}                                      | (a)
|-----------------|-----------------------------------------------|
|    (2, 1221)    | [points to list (c) in reference map]         | (d)
|-----------------|-----------------------------------------------|
|                 |                                               |
'-----------------'-----------------------------------------------'

key               value
 ________________ _______________________________________________
|SectionIndexKey | List<SafePointer*>                            |
|----------------|-----------------------------------------------|
|    (2, 2)      | {0x21232, 0x12122}                            | (b)
|----------------|-----------------------------------------------|
|    (2, 2)      | [points to list (c) in reference map]         | (d)
|----------------|-----------------------------------------------|
|                |                                               |
'----------------'-----------------------------------------------'

The Reference Map
-----------------

There's also a map which maps registered objects to lists of SafePointers
that are referencing these objects. This is called a reference map.

key             value
 ______________ ______________________________________
|SafePointable*| List<SafePointer*>*                  |
|--------------|--------------------------------------|
|   0x2212d    | {0x21232, 0x12122, 0x23e23, 0x2212}  | (c)
|--------------|--------------------------------------|
|              |                                      |
|--------------|--------------------------------------|
|              |                                      |
'--------------'--------------------------------------'

What happens when a SafePointer is requested for a reference to an object
that is not yet registered? A new SafePointer is created and a pointer to it
is placed in a list in correct position in correct map. Let's say the key
type was SectionOffsetKey and no one has requested a reference for it
yet. This causes a new entry to be added (a). The newly created SafePointer*
is placed in the List and set to point to null.

In another example case a reference is requested with a key that is already
in the map (SectionIndexKey(2,2)). Then the newly created SafePointer* is
simply appended to the existing list. The value (the pointer to the actual
object) of SafePointer* is copied from existing SafePointer(s) in the list.

Finally, an object reference is added. With object reference all three
different key types are kindly registered. This causes a new entry to be
added to the reference map with the object pointer as a key. While all keys
are registered all three key maps are traversed and the lists of
safepointers are merged to the list in reference map with the values of the
safepointers updated to point to the object that was registered.

The original entries (a) and (b) in SectionOffsetKey and SectionIndexKey,
respectively, are removed and replaced by a new entry (d) containing the the
list in (c).

Because there was no entry in the FileOffset key map for this object, a new
entry is entered in it.  Also, the list<SafePointer*> pointers in key maps
are set to point to the list we merged into the reference map (d). So in the
end we have a situation where

  SectionIndexKey(2,2),
  SectionOffsetKey(2, 1221)
  FileOffsetKey(1232212)

all point to the same list of SafePointers, which all point to the same
object.

Later when a SafePointer is requested for an object we only have to add the
newly created SafePointer to the list in the object's position in the
reference map.  When a notify of deletion of object is received we get the
list of SafePointers pointing to this object from the reference map and set
all of these SafePointers to point to null and remove the entry for the
deleted object from the reference map.

   Note for future: To ease the writing of binary files, methods for setting
   and getting keys for references might be useful.


SafePointers: Motivations
-------------------------

The idea of using safe pointers is to introduce an additional level of
indirection when referencing objects to allow to keep track of these
references and easily update them later on. We must use this kind of system
to make it possible to have references to an object that is yet to be
constructed.

Other advantage of SafePointers is that they avoid or reduce the problem of
dangling pointers.  Dangling pointers occur when multiple objects are
pointing directly to some object which is then deleted.  As a result, the
pointers are pointing to undefined memory. The idea of using indirection to
remove dangling pointers (by setting them to NULL automatically) is called
'tombstone' in some books and references.

By overloading '*' and '->' operators, these pointers might be used the same
way as real pointers thus being quite transparent to the user.  The
interfaces, however, remain obviously different.  Another problem of using
SafePointers in lieu of actual object pointers is that the type checking
becomes somewhat relaxed as a result.
