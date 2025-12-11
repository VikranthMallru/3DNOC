#include "types.hpp"
#include <string>
#include <map>


class c_TypeToString    {
private:
    static std::map<t_ChangeToXType,std::string> m_changeToXTypeMap;
    static std::map<t_RequestType,std::string> m_requestTypeMap;
    static std::map<t_ResponseType,std::string> m_responseTypeMap;
    static std::map<t_RequestQueuePriority,std::string> m_requestQueuePriorityMap;
    static std::map<t_LineType,std::string> m_lineTypeMap;
    static std::map<t_CacheState,std::string> m_stateTypeMap;
    static std::map<t_AddressType,std::string> m_addressTypeMap;
    static std::map<t_SHRType,std::string> m_bufferTypeMap;
    static std::map<t_QueueType,std::string> m_queueTypeMap;
    static std::map<t_FixedLatencyRequestType,std::string> m_fixedLatencyReqTypeMap;
    static std::map<t_ProbeType,std::string> m_probeTypeMap;
    static std::map<t_RequestControllerState,std::string> m_requestControllerStateMap;
    static std::map<t_LookUpStatus,std::string> m_lookupStatusMap;
    static std::map<t_MetadataControllerState,std::string> m_metadataControllerStateMap;
    static std::map<t_StateRequestType,std::string> m_stateRequestTypeMap;
    static std::map<t_StateResponseType,std::string> m_stateResponseTypeMap;
    static std::map<t_ReplacementPolicy,std::string> m_replPolicyMap;
    static std::map<t_IndexingPolicy,std::string> m_indexPolicyMap;
    static std::map<t_IssueType,std::string> m_issueTypeMap;
    static std::map<t_SwitchDirection,std::string> m_switchDirectionMap;
    static std::map<t_RepeaterDirection,std::string> m_repeaterDirectionMap;
    static std::map<t_DeviceType,std::string> m_deviceTypeMap;
public:
    static void load();
    static std::string f_changeToXTypeString(t_ChangeToXType m_changeToXType);
    static std::string f_requestTypeString(t_RequestType p_requestType);
    static std::string f_responseTypeString(t_ResponseType p_responseType);
    static std::string f_lineTypeString(t_LineType p_lineType);
    static std::string f_stateTypeString(t_CacheState p_state); 
    static std::string f_addressTypeString(t_AddressType p_addressType);
    static std::string f_bufferTypeString(t_SHRType p_bufferType);
    static std::string f_queueTypeString(t_QueueType p_queueType);
    static std::string f_fixedLatencyReqTypeString(t_FixedLatencyRequestType p_reqType);
    static std::string f_probeTypeString(t_ProbeType p_probeType);
    static std::string f_requestControllerStateString(t_RequestControllerState p_requestControllerState);
    static std::string f_lookUpStateString(t_LookUpStatus p_lookUpStatus);
    static std::string f_metadataControllerStateString(t_MetadataControllerState p_metadataCtrlState);
    static std::string f_stateRequestTypeString(t_StateRequestType p_stateRequestType);
    static std::string f_stateResponseTypeString(t_StateResponseType p_stateResponseType);
    static std::string f_replacementPolicyString(t_ReplacementPolicy p_replacementPolicy);
    static std::string f_indexPolicyString(t_IndexingPolicy p_indexingPolicy);
    static std::string f_issueTypeString(t_IssueType p_issueType);
    static std::string f_switchDirectionString(t_SwitchDirection p_direction);
    static std::string f_repeaterDirectionString(t_RepeaterDirection p_direction);
    static std::string f_deviceTypeString(t_DeviceType p_deviceType);
};