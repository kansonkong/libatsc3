//
//  MPUInspector.h
//  Bento4
//
//  Created by Jason Justman on 2/17/19.
//

#include "Ap4Types.h"
#include "Ap4List.h"
#include "Ap4ByteStream.h"
#include "Ap4String.h"
#include "Ap4Debug.h"
#include "Ap4DynamicCast.h"
#include "Ap4Array.h"
#include "Ap4Atom.h"

#ifndef MPUInspector_h
#define MPUInspector_h



class AP4_MPUInspector : public AP4_AtomInspector {
public:
    AP4_MPUInspector(AP4_ByteStream& stream, AP4_Cardinal indent=0);
    ~AP4_MPUInspector();
    
    // methods
    void StartAtom(const char* name,
                   AP4_UI08    version,
                   AP4_UI32    flags,
                   AP4_Size    header_size,
                   AP4_UI64    size);
    void EndAtom();
    void StartDescriptor(const char* name,
                         AP4_Size    header_size,
                         AP4_UI64    size);
    void EndDescriptor();
    void AddField(const char* name, AP4_UI64 value, FormatHint hint);
    void AddFieldF(const char* name, float value, FormatHint hint);
    void AddField(const char* name, const char* value, FormatHint hint);
    void AddField(const char* name, const unsigned char* bytes, AP4_Size size, FormatHint hint);
    
private:
    // members
    AP4_ByteStream* m_Stream;
    AP4_Cardinal    m_Indent;
};
#endif /* MPUInspector_h */
