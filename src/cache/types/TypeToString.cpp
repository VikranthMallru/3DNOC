#include "TypeToString.hpp"
#include <iostream>
#include <assert.h>


std::map<t_ChangeToXType,std::string> c_TypeToString::m_changeToXTypeMap;
std::map<t_RequestType,std::string> c_TypeToString::m_requestTypeMap;
std::map<t_ResponseType,std::string> c_TypeToString::m_responseTypeMap;
std::map<t_LineType,std::string> c_TypeToString::m_lineTypeMap;
std::map<t_CacheState,std::string> c_TypeToString::m_stateTypeMap;
std::map<t_AddressType,std::string> c_TypeToString::m_addressTypeMap;
std::map<t_SHRType,std::string> c_TypeToString::m_bufferTypeMap;
std::map<t_QueueType,std::string> c_TypeToString::m_queueTypeMap;
std::map<t_FixedLatencyRequestType,std::string> c_TypeToString::m_fixedLatencyReqTypeMap;
std::map<t_ProbeType,std::string> c_TypeToString::m_probeTypeMap;
std::map<t_RequestControllerState,std::string> c_TypeToString::m_requestControllerStateMap;
std::map<t_LookUpStatus,std::string> c_TypeToString::m_lookupStatusMap;
std::map<t_MetadataControllerState,std::string> c_TypeToString::m_metadataControllerStateMap;
std::map<t_StateRequestType,std::string> c_TypeToString::m_stateRequestTypeMap;
std::map<t_StateResponseType,std::string> c_TypeToString::m_stateResponseTypeMap;
std::map<t_ReplacementPolicy,std::string> c_TypeToString::m_replPolicyMap;
std::map<t_IndexingPolicy,std::string> c_TypeToString::m_indexPolicyMap;
std::map<t_IssueType,std::string> c_TypeToString::m_issueTypeMap;
std::map<t_SwitchDirection,std::string> c_TypeToString::m_switchDirectionMap;
std::map<t_RepeaterDirection,std::string> c_TypeToString::m_repeaterDirectionMap;
std::map<t_DeviceType,std::string> c_TypeToString::m_deviceTypeMap;

void c_TypeToString::load()  {
	m_lineTypeMap[E_NOLINETYPE] = "NLT";
	m_lineTypeMap[E_INSTRUCTION] = "INST";
	m_lineTypeMap[E_DATA] = "DATA";
	m_lineTypeMap[E_SMC] = "SMC ";
	m_lineTypeMap[E_NUM_LINETYPE] = "MLT";

    m_stateTypeMap[E_INVALID] = "I ";
	m_stateTypeMap[E_DATA_WAIT] = "DW";
	m_stateTypeMap[E_EXCLUSIVE] = "E ";
	m_stateTypeMap[E_SHD_EX] = "EP";
	m_stateTypeMap[E_SHARED] = "S ";
	m_stateTypeMap[E_MODIFIED] = "M ";
	m_stateTypeMap[E_NUM_STATES] = "NS";


	m_addressTypeMap[E_PIPT] = "PIPT";
	m_addressTypeMap[E_VIPT] = "VIPT";
	m_addressTypeMap[E_VIVT] = "VIVT";
	m_addressTypeMap[E_NUM_ADDR_TYPE] = "NA  ";

	m_changeToXTypeMap[E_C2X_REQ] = "C2XR";
	m_changeToXTypeMap[E_C2X_RESP] = "C2XRP";
	m_changeToXTypeMap[E_NO_C2X] = "NA";

	m_requestTypeMap[E_CORE_READ] = "RDREQ";
	m_requestTypeMap[E_CORE_WRITE] = "WRREQ";
	m_requestTypeMap[E_PREFETCH_RECV] = "PF";
	m_requestTypeMap[E_PROBE_REQ] = "PBR_REQ";
	m_requestTypeMap[E_PROBE_RESP] = "PBR_RSP";
	m_requestTypeMap[E_FILL] = "FILL";
	m_requestTypeMap[E_EVICT] = "EVICT";
	m_requestTypeMap[E_NUM_RECVQ_TYPE] = "NONE";

	m_responseTypeMap[E_READ_RESP] = "RDRESP";
	m_responseTypeMap[E_MISS] = "MISS";
	m_responseTypeMap[E_VICTIM] = "VICTIM";
	m_responseTypeMap[E_PROBE_REQ_SEND] = "PRB_REQ";
	m_responseTypeMap[E_PROBE_RESP_SEND] = "PBR_RESP";
	m_responseTypeMap[E_PREFETCH_SEND] = "PFTRAIN";
	m_responseTypeMap[E_NUM_SENDQ_TYPE] = "NONE";

	m_bufferTypeMap[E_READ_SHR] = " RD_SHR";
	m_bufferTypeMap[E_VICTIM_BUFFER] = "VIC_BUF";
	m_bufferTypeMap[E_PROBE_SHR] = "PRB_SHR";
	m_bufferTypeMap[E_ISSUE_BUFFER] = "ISS_SHR";
	m_bufferTypeMap[E_NUM_SHR_TYPE] = " NO_SHR";

	m_queueTypeMap[E_REQ_Q] = "REQ_Q";
	m_queueTypeMap[E_RESP_Q] = "RSP_Q";
	m_queueTypeMap[E_ISSUE_Q] = "ISS_Q";
	m_queueTypeMap[E_NUM_Q_TYPES] = "NA";

	m_issueTypeMap[E_DATA_READ] = "DRD";
	m_issueTypeMap[E_DATA_WRITE] = "DWR";
	m_issueTypeMap[E_TAG_WRITE] = "TWR";
	m_issueTypeMap[E_METADATA_RESP] = "MDR";

	m_fixedLatencyReqTypeMap[E_FIXED_LATENCY_DATA_READ] = "SRD";
	m_fixedLatencyReqTypeMap[E_FIXED_LATENCY_DATA_WRITE] = "SWR";
	m_fixedLatencyReqTypeMap[E_NUM_FIXED_LATENCY_REQTYPE] = "NA";
 
    m_probeTypeMap[E_PRB_NOTHING] = "PRB_NA";
	m_probeTypeMap[E_PRB_NOCHANGE] = "PRB_NC";
	m_probeTypeMap[E_PRB_INVALIDATE] = "PRB_INV";
	m_probeTypeMap[E_PRB_SHARED] = "PRB_SH";
	m_probeTypeMap[E_NUM_PROBETYPE] = "NA";
	

	m_requestControllerStateMap[E_RCS_IDLE] = "Idle";
	m_requestControllerStateMap[E_RCS_FETCHED] = "F";
	m_requestControllerStateMap[E_RCS_DECODED] = "D";
	m_requestControllerStateMap[E_RCS_ISSUED] = "I";
	m_requestControllerStateMap[E_RCS_EXECUTED] = "E";
	m_requestControllerStateMap[E_RCS_RETIRED] = "R";
	m_requestControllerStateMap[E_NUM_RCS_STATES] = "NA";

    m_lookupStatusMap[E_LUS_HIT] = "H";
	m_lookupStatusMap[E_LUS_MISS] = "M";
	m_lookupStatusMap[E_LUS_HIT_NO_PERM] = "HNP";
	m_lookupStatusMap[E_LUS_DIRECTORY_HIT] = "DH";
	m_lookupStatusMap[E_LUS_DIRECTORY_MISS] = "DM";
	m_lookupStatusMap[E_LUS_NA] = "NA";


	m_metadataControllerStateMap[E_MCS_IDLE] = "I";
	m_metadataControllerStateMap[E_MCS_TAG_READ_DONE] = "TRD";
	m_metadataControllerStateMap[E_MCS_DATA_READ_DONE] = "DRD";
	m_metadataControllerStateMap[E_MCS_DATA_WRITE_DONE] = "DWR";
	m_metadataControllerStateMap[E_MCS_TAG_WRITE_DONE] = "TWR";
	m_metadataControllerStateMap[E_MCS_RETIRED] = "R";
	m_metadataControllerStateMap[E_NUM_MCS_STATES] = "NA";

	m_stateRequestTypeMap[E_SRT_RESERVE] = "RESERVE";
	m_stateRequestTypeMap[E_SRT_READ] = "RD";
	m_stateRequestTypeMap[E_SRT_WRITE] = "WR";
	m_stateRequestTypeMap[E_SRT_PROBE_SHD] = "PRB_SH";
	m_stateRequestTypeMap[E_SRT_PROBE_INV] = "PRB_IN";
	m_stateRequestTypeMap[E_SRT_EVICT] = "EVICT";
	m_stateRequestTypeMap[E_SRT_EX_PER] = "EX_PER";
	m_stateRequestTypeMap[E_SRT_FILL] = "FILL";
	m_stateRequestTypeMap[E_NUM_STATE_REQTYPE] = "NA";

	m_stateResponseTypeMap[E_STATERESP_FAIL] = "FAIL";
	m_stateResponseTypeMap[E_STATERESP_DATA] = "DATA";
	m_stateResponseTypeMap[E_STATERESP_ACK] = "ACK";
	m_stateResponseTypeMap[E_STATERESP_MISS] = "MISS";
	m_stateResponseTypeMap[E_STATERESP_DATA_WB] = "DATA_WB";
	m_stateResponseTypeMap[E_NUM_STATE_RESPTYPE] = "NONE";

    m_replPolicyMap[E_REPL_POLICY_LRU] = "LRU";
	m_replPolicyMap[E_REPL_POLICY_FIFO_ALLOC] = "FIFOALLOC";
	m_replPolicyMap[E_REPL_POLICY_FIFO_FILL] = "FIFOFILL";
	m_replPolicyMap[E_REPL_POLICY_RANDOM] = "RAN";
	m_replPolicyMap[E_NUM_REPL_POLICY] = "NA";

	m_indexPolicyMap[E_INDEXING_NOHASH] = "NOHASH";
	m_indexPolicyMap[E_INDEXING_HASH] = "HASH";
	m_indexPolicyMap[E_NUM_INDEXING_POLICY] = "NA";

	m_switchDirectionMap[E_PE] = "PE";
	m_switchDirectionMap[E_NORTH] = "NORTH";
	m_switchDirectionMap[E_SOUTH] = "SOUTH";
	m_switchDirectionMap[E_EAST] = "EAST";
	m_switchDirectionMap[E_WEST] = "WEST";
	m_switchDirectionMap[E_UP] = "UP";
	m_switchDirectionMap[E_DOWN] = "DOWN";
	m_switchDirectionMap[E_MAX_DIR_SWITCH] = "NA";
	m_switchDirectionMap[E_MM] = "MM";

	m_repeaterDirectionMap[E_NEXT] = "NEXT";
	m_repeaterDirectionMap[E_PREV] = "PREV";

	m_deviceTypeMap[E_SWITCH] = "SWITCH";
	m_deviceTypeMap[E_REPEATER] = "REPEATER";
	m_deviceTypeMap[E_DUMMY] = "DUMMY";
}

std::string c_TypeToString::f_issueTypeString(t_IssueType p_issueType)	{
	try	{
		return m_issueTypeMap.at(p_issueType);
	}
	catch(const std::exception& e)	{
		std::cout << "Issue type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_changeToXTypeString(t_ChangeToXType p_changeToXType)	{
	try	{
		return m_changeToXTypeMap.at(p_changeToXType);
	}
	catch(const std::exception& e)	{
		std::cout << "ChgToX List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_requestTypeString(t_RequestType p_requestType) {
	try	{
	    return m_requestTypeMap.at(p_requestType);
	}
	catch(const std::exception& e)	{
		std::cout << "Request Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_responseTypeString(t_ResponseType p_responseType)  {
	try	{
	    return m_responseTypeMap.at(p_responseType);
	}
	catch(const std::exception& e)	{
		std::cout << "Response Type List Issue: "<<p_responseType;
		assert(false);
	}
}

std::string c_TypeToString::f_lineTypeString(t_LineType p_lineType) {
	try	{
		return m_lineTypeMap.at(p_lineType);
		}
	catch(const std::exception& e)	{
		std::cout << "Line Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_stateTypeString(t_CacheState p_state)  {
	try	{
	    return m_stateTypeMap.at(p_state);
	}
	catch(const std::exception& e)	{
		std::cout << "State Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_addressTypeString(t_AddressType p_addressType)    {
	try	{
		return m_addressTypeMap.at(p_addressType);
	}
	catch(const std::exception& e)	{
		std::cout << "Address Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_bufferTypeString(t_SHRType p_bufferType) {
	try	{
		return m_bufferTypeMap.at(p_bufferType);
	}
	catch(const std::exception& e)	{
		std::cout << "Buffer Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_queueTypeString(t_QueueType p_queueType)  {
	try	{
		return m_queueTypeMap.at(p_queueType);
	}
	catch(const std::exception& e)	{
		std::cout << "Queue Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_fixedLatencyReqTypeString(t_FixedLatencyRequestType p_reqType) {
	try	{
		return m_fixedLatencyReqTypeMap.at(p_reqType);
	}
	catch(const std::exception& e)	{
		std::cout << "Fixed Latency Req Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_probeTypeString(t_ProbeType p_probeType)  {
	try	{
	    return m_probeTypeMap.at(p_probeType);
	}
	catch(const std::exception& e)	{
		std::cout << "Probe Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_requestControllerStateString(t_RequestControllerState p_requestControllerState) {
	try	{
		return m_requestControllerStateMap.at(p_requestControllerState);
	}
	catch(const std::exception& e)	{
		std::cout << "Request Controller State Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_lookUpStateString(t_LookUpStatus p_lookUpStatus)    {
	try	{
	    return m_lookupStatusMap.at(p_lookUpStatus);
	}
	catch(const std::exception& e)	{
		std::cout << "Lookup Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_metadataControllerStateString(t_MetadataControllerState p_metadataCtrlState)    {
	try	{
		return m_metadataControllerStateMap.at(p_metadataCtrlState);
	}
	catch(const std::exception& e)	{
		std::cout << "Metadata Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_stateRequestTypeString(t_StateRequestType p_stateRequestType)   {
	try	{
		return m_stateRequestTypeMap.at(p_stateRequestType);
	}
	catch(const std::exception& e)	{
		std::cout << "State Request Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_stateResponseTypeString(t_StateResponseType p_stateResponseType)  {
	try	{
		return m_stateResponseTypeMap.at(p_stateResponseType);
	}
	catch(const std::exception& e)	{
		std::cout << "State Response Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_replacementPolicyString(t_ReplacementPolicy p_replacementPolicy)  {
	try	{
	    return m_replPolicyMap.at(p_replacementPolicy);
	}
	catch(const std::exception& e)	{
		std::cout << "Replacement Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_indexPolicyString(t_IndexingPolicy p_indexingPolicy)    {
	try	{
		return m_indexPolicyMap.at(p_indexingPolicy);
	}
	catch(const std::exception& e)	{
		std::cout << "Index Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_switchDirectionString(t_SwitchDirection p_direction)	{
	try	{
		return m_switchDirectionMap.at(p_direction);
	}
	catch(const std::exception& e)	{
		std::cout << "Switch Direction Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_repeaterDirectionString(t_RepeaterDirection p_direction)	{
	try	{
		return m_repeaterDirectionMap.at(p_direction);
	}
	catch(const std::exception& e)	{
		std::cout << "Repeater Type List Issue";
		assert(false);
	}
}

std::string c_TypeToString::f_deviceTypeString(t_DeviceType p_deviceType) 	{
	try	{
		return m_deviceTypeMap.at(p_deviceType);
	}
	catch(const std::exception& e)	{
		std::cout << "Device Type List Issue";
		assert(false);
	}
}
