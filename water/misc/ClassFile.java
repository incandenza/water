// ClassFile
// by Dan McGuirk <mcguirk@indirect.com> 11/23/96

// parses a Java .class file and lets you examine the sweet
// gooey innards.

// based on the 1.0.2 virtual machine spec from www.javasoft.com.
// (which is also a chapter of the book _The Java Virtual Machine_.)

import java.io.*;

class ClassFile {

    // members of the ClassFile structure as described in the spec

    byte[] magic;
    short minor_version;
    short major_version;
    short constant_pool_count;
    ConstantPoolInfo[] constant_pool;
    short access_flags;
    short this_class;
    short super_class;
    short interfaces_count;
    short[] interfaces;
    short fields_count;
    FieldInfo[] fields;
    short methods_count;
    MethodInfo[] methods;
    short attributes_count;
    AttributeInfo[] attributes;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);
	    int i;

	    magic = new byte[4];
	    for(i=0; i < 4; i++) {
		magic[i] = data_is.readByte();
	    }
	    // the magic number should be 0xCAFEBABE
	    if(magic[0] != (byte)0xca ||
	       magic[1] != (byte)0xfe ||
	       magic[2] != (byte)0xba ||
	       magic[3] != (byte)0xbe) {
		throw new BadClassFileException("bad magic number.  magic number is " + magic[0] + magic[1] + magic[2] + magic[3]);
	    }

	    minor_version = data_is.readShort();
	    major_version = data_is.readShort();

	    constant_pool_count = data_is.readShort();

	    // leave the zeroth, 'reserved for internal VM use'
	    // entry unused, just so our indexes match up
	    constant_pool = new ConstantPoolInfo[constant_pool_count];

	    // we must start at 1 here when reading in the actual constants
	    for(i = 1; i < constant_pool_count; i++) {
		constant_pool[i] = new ConstantPoolInfo();
		constant_pool[i].setClassFile(this);
		constant_pool[i].read(is);

		// if the constant pool entry is of type double or long,
		// we have to skip over the next constant table entry.
		// the spec says, "In retrospect, making eight-byte constants
		// take two constant pool entries was a poor choice."
		byte tag = constant_pool[i].getTag();
		if(tag == ConstantFactory.ConstantDoubleTag ||
		   tag == ConstantFactory.ConstantLongTag) {
		    i++;
		}
	    }

	    access_flags = data_is.readShort();
	    this_class = data_is.readShort();
	    super_class = data_is.readShort();

	    interfaces_count = data_is.readShort();
	    interfaces = new short[interfaces_count];
	    for(i=0; i < interfaces_count; i++) {
		// the interfaces are just normal shorts--indexes
		// of CONSTANT_Class_info entries in the constant pool
		interfaces[i] = data_is.readShort();
	    }

	    fields_count = data_is.readShort();
	    fields = new FieldInfo[fields_count];
	    for(i=0; i < fields_count; i++) {
		fields[i] = new FieldInfo();
		fields[i].setClassFile(this);
		fields[i].read(is);
	    }

	    methods_count = data_is.readShort();
	    methods = new MethodInfo[methods_count];
	    for(i=0; i < methods_count; i++) {
		methods[i] = new MethodInfo();
		methods[i].setClassFile(this);
		methods[i].read(is);
	    }

	    attributes_count = data_is.readShort();
	    attributes = new AttributeInfo[attributes_count];
	    for(i=0; i < attributes_count; i++) {
		attributes[i] = new AttributeInfo();		
		attributes[i].setClassFile(this);
		attributes[i].read(is);
	    }
	}

    public ConstantPoolInfo getConstantPoolEntry(int entry_number) {
	return constant_pool[entry_number];
    }
}

class ConstantPoolInfo {

    // an entry in the constant pool.

    ClassFile ourClassFile;

    byte tag;
    GenericConstant info;

    public void read(InputStream is) 
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    tag = data_is.readByte();

	    try {
		info = ConstantFactory.create(tag);
	    } catch(BadConstantTagException e) {
		throw new BadClassFileException("got an unrecognized constant tag: " + tag);
	    }

	    info.read(is);
	}	    

    public byte getTag() {
	return tag;
    }

    public GenericConstant getConstant() {
	return info;
    }

    public void setClassFile(ClassFile classFile) {
	ourClassFile = classFile;
    }
}

// MethodInfo and FieldInfo are basically the same, except for the different
// access flags.  so derive them from the same base.

class FieldOrMethodInfo {

    ClassFile ourClassFile;

    short access_flags;
    short name_index;
    short descriptor_index;
    short attributes_count;
    AttributeInfo[] attributes;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    access_flags = data_is.readShort();
	    name_index = data_is.readShort();
	    descriptor_index = data_is.readShort();

	    attributes_count = data_is.readShort();

	    attributes = new AttributeInfo[attributes_count];
	    for(int i=0; i < attributes_count; i++) {
		attributes[i] = new AttributeInfo();
		attributes[i].setClassFile(ourClassFile);
		attributes[i].read(is);
	    }
	}

    public void setClassFile(ClassFile classFile) {
	ourClassFile = classFile;
    }
}

class FieldInfo extends FieldOrMethodInfo {
    // nothing to add here
}

class MethodInfo extends FieldOrMethodInfo {
    // nothing to add here
}

class AttributeInfo {

    ClassFile ourClassFile;

    short attribute_name;
    // this time the length goes inside the GenericAttribute
    GenericAttribute attribute;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    attribute_name = data_is.readShort();

	    // get the constant out of the constant pool and find out the real name
	    ConstantPoolInfo cp_info =
	      ourClassFile.getConstantPoolEntry(attribute_name);
	    if(cp_info.getTag() != ConstantFactory.ConstantUtf8Tag) {
		throw new BadClassFileException("index " + attribute_name + " is supposed to be an attribute, but it's not of type Utf8 (actual type is " + cp_info.getTag() + ")");
	    }

	    String attribute_name_string = 
	      ((ConstantUtf8)cp_info.getConstant()).getValue();

	    attribute = AttributeFactory.create(attribute_name_string);
	    attribute.setClassFile(ourClassFile);
	    attribute.read(is);
	}

    public void setClassFile(ClassFile classFile) {
	ourClassFile = classFile;
    }
}

class AttributeFactory {

    public static GenericAttribute create(String attribute_type) {
	if(attribute_type.equals("SourceFile")) {
	    return new SourceFileAttribute();
	} else if(attribute_type.equals("ConstantValue")) {
	    return new ConstantValueAttribute();
	} else if(attribute_type.equals("Code")) {
	    return new CodeAttribute();
	} else if(attribute_type.equals("Exceptions")) {
	    return new ExceptionsAttribute();
	} else if(attribute_type.equals("LineNumberTable")) {
	    return new LineNumberTableAttribute();
	} else if(attribute_type.equals("LocalVariableTable")) {
	    return new LocalVariableTableAttribute();
	} else {
	    return new UnrecognizedAttribute();
	}
    }
}

abstract class GenericAttribute {

    ClassFile ourClassFile;

    int attribute_length;

    public void read(InputStream is) 
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    attribute_length = data_is.readInt();
	}

    public void setClassFile(ClassFile classFile) {
	ourClassFile = classFile;
    }
}

class SourceFileAttribute extends GenericAttribute {

    short sourcefile_index;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);
	    super.read(is);

	    sourcefile_index = data_is.readShort();
	}
}

class ConstantValueAttribute extends GenericAttribute {

    short constantvalue_index;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);
	    super.read(is);
	    
	    constantvalue_index = data_is.readShort();
	}
}

class CodeAttribute extends GenericAttribute {

    // here's where the good stuff is
    short max_stack;
    short max_locals;
    int code_length;
    byte[] code;
    short exception_table_length;
    ExceptionTableEntry[] exception_table;
    short attributes_count;
    AttributeInfo[] attributes;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);
	    int i;
	    super.read(is);
	    
	    max_stack = data_is.readShort();
	    max_locals = data_is.readShort();
	    code_length = data_is.readInt();
	    code = new byte[code_length];
	    is.read(code, 0, code_length);
	    
	    exception_table_length = data_is.readShort();
	    exception_table = new ExceptionTableEntry[exception_table_length];
	    for(i=0; i < exception_table_length; i++) {
		exception_table[i] = new ExceptionTableEntry();
		exception_table[i].read(is);
	    }					    

	    attributes_count = data_is.readShort();
	    attributes = new AttributeInfo[attributes_count];
	    for(i=0; i < attributes_count; i++) {
		attributes[i] = new AttributeInfo();
		attributes[i].setClassFile(ourClassFile);
		attributes[i].read(is);
	    }
	}
}

class ExceptionTableEntry {

    short start_pc;
    short end_pc;
    short handler_pc;
    short catch_type;

    public void read(InputStream is)
      throws IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    start_pc = data_is.readShort();
	    end_pc = data_is.readShort();
	    handler_pc = data_is.readShort();
	    catch_type = data_is.readShort();
	}
}

class ExceptionsAttribute extends GenericAttribute {

    short number_of_exceptions;
    short[] exception_table_index;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);
	    super.read(is);

	    number_of_exceptions = data_is.readShort();
	    exception_table_index = new short[number_of_exceptions];
	    for(int i=0; i < number_of_exceptions; i++) {
		exception_table_index[i] = data_is.readShort();
	    }
	}
}

class LineNumberTableAttribute extends GenericAttribute {

    short line_number_table_length;
    LineNumberTableEntry[] line_number_table;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);
	    super.read(is);

	    line_number_table_length = data_is.readShort();
	    line_number_table = new LineNumberTableEntry[line_number_table_length];
	    for(int i=0; i < line_number_table_length; i++) {
		line_number_table[i] = new LineNumberTableEntry();
		line_number_table[i].read(is);
	    }
	}
}

class LineNumberTableEntry {

    short start_pc;
    short line_number;

    public void read(InputStream is)
      throws IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    start_pc = data_is.readShort();
	    line_number = data_is.readShort();
	}
}

class LocalVariableTableAttribute extends GenericAttribute {

    short local_variable_table_length;
    LocalVariableTableEntry[] local_variable_table;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);
	    super.read(is);

	    local_variable_table_length = data_is.readShort();
	    local_variable_table = new LocalVariableTableEntry[local_variable_table_length];
	    for(int i=0; i < local_variable_table_length; i++) {
		local_variable_table[i] = new LocalVariableTableEntry();
		local_variable_table[i].read(is);
	    }
	}
}

class LocalVariableTableEntry {

    short start_pc;
    short length;
    short name_index;
    short descriptor_index;
    short index;

    public void read(InputStream is)
      throws IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    start_pc = data_is.readShort();
	    length = data_is.readShort();
	    name_index = data_is.readShort();
	    descriptor_index = data_is.readShort();
	    index = data_is.readShort();
	}
}

class UnrecognizedAttribute extends GenericAttribute {
    // an attribute we don't recognize.  just store the raw contents.

    byte[] unrecognized_data;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    super.read(is);
	    unrecognized_data = new byte[attribute_length];
	    is.read(unrecognized_data, 0, attribute_length);
	}
}

abstract class GenericConstant {
    // an abstract base class for the different types of constants

    public abstract void read(InputStream is) throws ClassFileException, IOException;
}

class ConstantFactory {
    // a factory class, used to spit out specific constant types
    // based on a tag type.

    public static final byte ConstantClassTag               =  7;
    public static final byte ConstantFieldrefTag            =  9;
    public static final byte ConstantMethodrefTag           = 10;
    public static final byte ConstantInterfaceMethodrefTag  = 11;
    public static final byte ConstantStringTag              =  8;
    public static final byte ConstantIntegerTag             =  3;
    public static final byte ConstantFloatTag               =  4;
    public static final byte ConstantLongTag                =  5;
    public static final byte ConstantDoubleTag              =  6;
    public static final byte ConstantNameAndTypeTag         = 12;
    public static final byte ConstantUtf8Tag                =  1;

    public static GenericConstant create(byte tag) 
      throws ClassFileException
	{
	    switch(tag) {
	      case(ConstantClassTag):              { return new ConstantClass(); }
	      case(ConstantFieldrefTag):           { return new ConstantFieldref(); }
	      case(ConstantMethodrefTag):          { return new ConstantMethodref(); }
	      case(ConstantInterfaceMethodrefTag): 
	          { return new ConstantInterfaceMethodref(); }
	      case(ConstantStringTag):             { return new ConstantString(); }
	      case(ConstantIntegerTag):            { return new ConstantInteger(); }
	      case(ConstantFloatTag):              { return new ConstantFloat(); }
	      case(ConstantLongTag):               { return new ConstantLong(); }
	      case(ConstantDoubleTag):             { return new ConstantDouble(); }
	      case(ConstantNameAndTypeTag):        
                  { return new ConstantNameAndType(); }
	      case(ConstantUtf8Tag):               { return new ConstantUtf8(); }
	      default:
		  throw new BadConstantTagException("got bad tag " + tag);
	    }
	}	
}

class ConstantClass extends GenericConstant {

    short name_index;

    public void read(InputStream is) 
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    name_index = data_is.readShort();
	}
}

class ConstantGenericref extends GenericConstant {

    // Fieldref's, Methodref's, and InterfaceMethodref's all contain
    // exactly the same data.  so make them all identical to this class.

    short class_index;
    short name_and_type_index;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    class_index = data_is.readShort();
	    name_and_type_index = data_is.readShort();
	}
}

class ConstantFieldref extends ConstantGenericref {
    // nothing to add here
}

class ConstantMethodref extends ConstantGenericref {
    // nothing to add here
}

class ConstantInterfaceMethodref extends ConstantGenericref {
    // nothing to add here
}

class ConstantString extends GenericConstant {

    short string_index;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    string_index = data_is.readShort();
	}
}

class ConstantInteger extends GenericConstant {

    int value;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    value = data_is.readInt();
	}
}

class ConstantFloat extends GenericConstant {

    float value;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    value = data_is.readFloat();
	}
}

class ConstantLong extends GenericConstant {
    
    long value;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    value = data_is.readLong();
	}
}

class ConstantDouble extends GenericConstant {

    double value;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    value = data_is.readDouble();
	}
}

class ConstantNameAndType extends GenericConstant {

    short name_index;
    short descriptor_index;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    name_index = data_is.readShort();
	    descriptor_index = data_is.readShort();
	}
}


class ConstantUtf8 extends GenericConstant {

    String value;

    public void read(InputStream is)
      throws ClassFileException, IOException
	{
	    DataInputStream data_is = new DataInputStream(is);

	    value = data_is.readUTF();
	}

    public String getValue() {
	return value;
    }
}

class ClassFileException extends Exception {
    public ClassFileException(String message) {
	super(message);
    }
}

class BadClassFileException extends ClassFileException {
    public BadClassFileException(String message) {
	super(message);
    }
}

class BadConstantTagException extends ClassFileException {
    public BadConstantTagException(String message) {
	super(message);
    }
}

class BadAttributeException extends ClassFileException {
    public BadAttributeException(String message) {
	super(message);
    }
}

class BadConstantException extends ClassFileException {
    public BadConstantException(String message) {
	super(message);
    }
}
