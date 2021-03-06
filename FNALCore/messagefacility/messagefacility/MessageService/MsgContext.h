#ifndef MessageFacility_MessageService_MsgContext_h
#define MessageFacility_MessageService_MsgContext_h

#include "messagefacility/MessageService/ELcontextSupplier.h"

#include <string>

namespace mf {
namespace service {

  class MsgContext : public ELcontextSupplier
  {
  public:
    MsgContext* clone() const { return new MsgContext(*this); }
    ELstring context() const { return context_; }
    ELstring summaryContext() const { return summary_context_; }
    ELstring fullContext() const { return context(); }

    void setContext(const std::string& c)
    {
      context_ = c;
      summary_context_ = compress(c);
    }
    void clearContext() { context_.clear(); summary_context_.clear(); }

  private:
    std::string context_;
    std::string summary_context_;
    std::string compress (const std::string& c) const;
  };

}        // end of namespace service
}       // end of namespace mf

#endif // MessageFacility_MessageService_MsgContext_h
