/**
 * @file AddressSpaceMapperPass.hh
 *
 * Declaration of AddressSpaceMapperPass class.
 * 
 * Address space mapper pass.
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADDRESS_SPACE_MAPPER_PASS_HH
#define TTA_ADDRESS_SPACE_MAPPER_PASS_HH

#include "AddressSpaceMapper.hh"
#include "StartableSchedulerModule.hh"

class AddressSpaceMapperPass : public StartableSchedulerModule {
public:
    AddressSpaceMapperPass();
    virtual ~AddressSpaceMapperPass();
    virtual void start()        
        throw (Exception);
    bool needsProgram() const { return true; }
    bool needsTarget() const { return true; }
private:
    AddressSpaceMapper mapperCore_;
};



#endif
