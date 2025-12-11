`include "types.sv"

// useless module as strings are non synthesisable

module c_TypeToString;

    string m_changeToXTypeMap[t_ChangeToXType];
    string m_requestTypeMap[t_RequestType];
    string m_responseTypeMap[t_ResponseType];
    string m_requestQueuePriorityMap[t_RequestQueuePriority];
    string m_lineTypeMap[t_LineType];
    string m_stateTypeMap[t_CacheState];
    string m_addressTypeMap[t_AddressType];
    string m_bufferTypeMap[t_SHRType];
    string m_queueTypeMap[t_QueueType];
    string m_fixedLatencyReqTypeMap[t_FixedLatencyRequestType];
    string m_probeTypeMap[t_ProbeType];
    string m_requestControllerStateMap[t_RequestControllerState];
    string m_lookupStatusMap[t_LookUpStatus];
    string m_metadataControllerStateMap[t_MetadataControllerState];
    string m_stateRequestTypeMap[t_StateRequestType];
    string m_stateResponseTypeMap[t_StateResponseType];
    string m_replPolicyMap[t_ReplacementPolicy];
    string m_indexPolicyMap[t_IndexingPolicy];
    string m_issueTypeMap[t_IssueType];
    string m_switchDirectionMap[t_SwitchDirection];
    string m_repeaterDirectionMap[t_RepeaterDirection];
    string m_deviceTypeMap[t_DeviceType];

    function void load();
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
    endfunction

    function string f_issueTypeString(t_IssueType p_issueType);
        return m_issueTypeMap.exists(p_issueType) ? m_issueTypeMap[p_issueType] : "Issue type List Issue";   
    endfunction

    function string f_changeToXTypeString(t_ChangeToXType p_changeToXType);
        return m_changeToXTypeMap.exists(p_changeToXType) ? m_changeToXTypeMap[p_changeToXType] : "ChgToX List Issue";
    endfunction
    
    function string f_requestTypeString(t_RequestType p_requestType) {
        return m_requestTypeMap.exists(p_requestType) ? m_requestTypeMap[p_requestType] : "Request Type List Issue";
    endfunction

        function string f_responseTypeString(t_ResponseType p_responseType);
        return m_responseTypeMap.exists(p_responseType) ? m_responseTypeMap[p_responseType] : "Response Type List Issue";
    endfunction

    function string f_lineTypeString(t_LineType p_lineType);
        return m_lineTypeMap.exists(p_lineType) ? m_lineTypeMap[p_lineType] : "Line Type List Issue";
    endfunction

    function string f_stateTypeString(t_CacheState p_state);
        return m_stateTypeMap.exists(p_state) ? m_stateTypeMap[p_state] : "State Type List Issue";
    endfunction

    function string f_addressTypeString(t_AddressType p_addressType);
        return m_addressTypeMap.exists(p_addressType) ? m_addressTypeMap[p_addressType] : "Address Type List Issue";
    endfunction

    function string f_bufferTypeString(t_SHRType p_bufferType);
        return m_bufferTypeMap.exists(p_bufferType) ? m_bufferTypeMap[p_bufferType] : "Buffer Type List Issue";
    endfunction

    function string f_queueTypeString(t_QueueType p_queueType);
        return m_queueTypeMap.exists(p_queueType) ? m_queueTypeMap[p_queueType] : "Queue Type List Issue";
    endfunction

    function string f_fixedLatencyReqTypeString(t_FixedLatencyRequestType p_reqType);
        return m_fixedLatencyReqTypeMap.exists(p_reqType) ? m_fixedLatencyReqTypeMap[p_reqType] : "Fixed Latency Req Type List Issue";
    endfunction

    function string f_probeTypeString(t_ProbeType p_probeType);
        return m_probeTypeMap.exists(p_probeType) ? m_probeTypeMap[p_probeType] : "Probe Type List Issue";
    endfunction

    function string f_requestControllerStateString(t_RequestControllerState p_requestControllerState);
        return m_requestControllerStateMap.exists(p_requestControllerState) ? m_requestControllerStateMap[p_requestControllerState] : "Request Controller State Type List Issue";
    endfunction

    function string f_lookUpStateString(t_LookUpStatus p_lookUpStatus);
        return m_lookupStatusMap.exists(p_lookUpStatus) ? m_lookupStatusMap[p_lookUpStatus] : "Lookup Type List Issue";
    endfunction

    function string f_metadataControllerStateString(t_MetadataControllerState p_metadataCtrlState);
        return m_metadataControllerStateMap.exists(p_metadataCtrlState) ? m_metadataControllerStateMap[p_metadataCtrlState] : "Metadata Type List Issue";
    endfunction

    function string f_stateRequestTypeString(t_StateRequestType p_stateRequestType);
        return m_stateRequestTypeMap.exists(p_stateRequestType) ? m_stateRequestTypeMap[p_stateRequestType] : "State Request Type List Issue";
    endfunction

    function string f_stateResponseTypeString(t_StateResponseType p_stateResponseType);
        return m_stateResponseTypeMap.exists(p_stateResponseType) ? m_stateResponseTypeMap[p_stateResponseType] : "State Response Type List Issue";
    endfunction

    function string f_replacementPolicyString(t_ReplacementPolicy p_replacementPolicy);
        return m_replPolicyMap.exists(p_replacementPolicy) ? m_replPolicyMap[p_replacementPolicy] : "Replacement Type List Issue";
    endfunction

    function string f_indexPolicyString(t_IndexingPolicy p_indexingPolicy);
        return m_indexPolicyMap.exists(p_indexPolicy) ? m_indexPolicyMap[p_indexPolicy] : "Index Type List Issue";
    endfunction

    function string f_switchDirectionString(t_SwitchDirection p_direction);
        return m_switchDirectionMap.exists(p_direction) ? m_switchDirectionMap[p_direction] : "Switch Direction Type List Issue";
    endfunction

    function string f_repeaterDirectionString(t_RepeaterDirection p_direction);
        return m_repeaterDirectionMap.exists(p_direction) ? m_repeaterDirectionMap[p_direction] : "Repeater Type List Issue";
    endfunction

    function string f_deviceTypeString(t_DeviceType p_deviceType);
        return m_deviceTypeMap.exists(p_deviceType) ? m_deviceTypeMap[p_deviceType] : "Device Type List Issue";
    endfunction

    
endmodule