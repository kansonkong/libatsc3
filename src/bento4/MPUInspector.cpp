//
//  MPUInspector.cpp
//  Bento4
//
//  Created by Jason Justman on 2/17/19.
//

#include <stdio.h>

#include "MPUInspector.h"
#include "Ap4Atom.h"
#include "Ap4Types.h"
#include "Ap4Atom.h"
#include "Ap4Utils.h"
#include "Ap4ContainerAtom.h"
#include "Ap4AtomFactory.h"
#include "Ap4Debug.h"
#include "Ap4UuidAtom.h"

/*----------------------------------------------------------------------
 |   AP4_MPUInspector::AP4_MPUInspector
 +---------------------------------------------------------------------*/
AP4_MPUInspector::AP4_MPUInspector(AP4_ByteStream& stream, AP4_Cardinal indent) :
m_Stream(&stream),
m_Indent(indent)
{
    m_Stream->AddReference();
}

/*----------------------------------------------------------------------
 |   AP4_MPUInspector::~AP4_MPUInspector
 +---------------------------------------------------------------------*/
AP4_MPUInspector::~AP4_MPUInspector()
{
    m_Stream->Release();
}


static void
AP4_MakePrefixString(unsigned int indent, char* prefix, AP4_Size size)
{
    if (size == 0) return;
    if (indent >= size-1) indent = size-1;
    for (unsigned int i=0; i<indent; i++) {
        prefix[i] = ' ';
    }
    prefix[indent] = '\0';
}
/*----------------------------------------------------------------------
 |   AP4_MPUInspector::StartAtom
 +---------------------------------------------------------------------*/
void
AP4_MPUInspector::StartAtom(const char* name,
                              AP4_UI08    version,
                              AP4_UI32    flags,
                              AP4_Size    header_size,
                              AP4_UI64    size)
{
    // write atom name
    char info[128];
    char extra[32] = "";
    if (header_size == 28 || header_size == 12 || header_size == 20) {
        if (version && flags) {
            AP4_FormatString(extra, sizeof(extra),
                             ", version=%d, flags=%x",
                             version,
                             flags);
        } else if (version) {
            AP4_FormatString(extra, sizeof(extra),
                             ", version=%d",
                             version);
        } else if (flags) {
            AP4_FormatString(extra, sizeof(extra),
                             ", flags=%x",
                             flags);
        }
    }
    AP4_FormatString(info, sizeof(info),
                     "size=%d+%lld%s",
                     header_size,
                     size-header_size,
                     extra);
    
    char prefix[256];
    AP4_MakePrefixString(m_Indent, prefix, sizeof(prefix));
    m_Stream->WriteString(prefix);
    m_Stream->WriteString("[");
    m_Stream->WriteString(name);
    m_Stream->Write("] ", 2);
    m_Stream->WriteString(info);
    m_Stream->Write("\n", 1);
    
    m_Indent += 2;
}

/*----------------------------------------------------------------------
 |   AP4_MPUInspector::EndAtom
 +---------------------------------------------------------------------*/
void
AP4_MPUInspector::EndAtom()
{
    m_Indent -= 2;
}

/*----------------------------------------------------------------------
 |   AP4_MPUInspector::StartDescriptor
 +---------------------------------------------------------------------*/
void
AP4_MPUInspector::StartDescriptor(const char* name,
                                    AP4_Size    header_size,
                                    AP4_UI64    size)
{
    // write atom name
    char info[128];
    AP4_FormatString(info, sizeof(info),
                     "size=%d+%lld",
                     header_size,
                     size-header_size);
    
    char prefix[256];
    AP4_MakePrefixString(m_Indent, prefix, sizeof(prefix));
    m_Stream->WriteString(prefix);
    m_Stream->Write("[", 1);
    m_Stream->WriteString(name);
    m_Stream->Write("] ", 2);
    m_Stream->WriteString(info);
    m_Stream->Write("\n", 1);
    
    m_Indent += 2;
}

/*----------------------------------------------------------------------
 |   AP4_MPUInspector::EndDescriptor
 +---------------------------------------------------------------------*/
void
AP4_MPUInspector::EndDescriptor()
{
    m_Indent -= 2;
}

/*----------------------------------------------------------------------
 |   AP4_MPUInspector::AddField
 +---------------------------------------------------------------------*/
void
AP4_MPUInspector::AddField(const char* name, const char* value, FormatHint)
{
    char prefix[256];
    AP4_MakePrefixString(m_Indent, prefix, sizeof(prefix));
    m_Stream->WriteString(prefix);
    
    m_Stream->WriteString(name);
    m_Stream->WriteString(" = ");
    m_Stream->WriteString(value);
    m_Stream->Write("\n", 1);
}

/*----------------------------------------------------------------------
 |   AP4_MPUInspector::AddField
 +---------------------------------------------------------------------*/
void
AP4_MPUInspector::AddField(const char* name, AP4_UI64 value, FormatHint hint)
{
    char prefix[256];
    AP4_MakePrefixString(m_Indent, prefix, sizeof(prefix));
    m_Stream->WriteString(prefix);
    
    char str[32];
    AP4_FormatString(str, sizeof(str),
                     hint == HINT_HEX ? "%llx":"%lld",
                     value);
    m_Stream->WriteString(name);
    m_Stream->WriteString(" = ");
    m_Stream->WriteString(str);
    m_Stream->Write("\n", 1);
}

/*----------------------------------------------------------------------
 |   AP4_MPUInspector::AddFieldF
 +---------------------------------------------------------------------*/
void
AP4_MPUInspector::AddFieldF(const char* name, float value, FormatHint /*hint*/)
{
    char prefix[256];
    AP4_MakePrefixString(m_Indent, prefix, sizeof(prefix));
    m_Stream->WriteString(prefix);
    
    char str[32];
    AP4_FormatString(str, sizeof(str),
                     "%f",
                     value);
    m_Stream->WriteString(name);
    m_Stream->WriteString(" = ");
    m_Stream->WriteString(str);
    m_Stream->Write("\n", 1);
}

/*----------------------------------------------------------------------
 |   AP4_MPUInspector::AddField
 +---------------------------------------------------------------------*/
void
AP4_MPUInspector::AddField(const char*          name,
                             const unsigned char* bytes,
                             AP4_Size             byte_count,
                             FormatHint           /* hint */)
{
    char prefix[256];
    AP4_MakePrefixString(m_Indent, prefix, sizeof(prefix));
    m_Stream->WriteString(prefix);
    
    m_Stream->WriteString(name);
    m_Stream->WriteString(" = [");
    unsigned int offset = 1;
    char byte[4];
    for (unsigned int i=0; i<byte_count; i++) {
        AP4_FormatString(byte, 4, " %02x", bytes[i]);
        m_Stream->Write(&byte[offset], 3-offset);
        offset = 0;
    }
    m_Stream->Write("]\n", 2);
}

