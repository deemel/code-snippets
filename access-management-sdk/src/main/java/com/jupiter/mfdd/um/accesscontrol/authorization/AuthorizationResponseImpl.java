package com.jupiter.mfdd.um.accesscontrol.authorization;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

import javax.ws.rs.core.GenericType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.Response.Status;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.authorization.AuthorizationPolicyDecision;
import com.jupiter.boss.sdk.accesscontrol.authorization.AuthorizationResponse;
import com.jupiter.mfdd.um.accesscontrol.authorization.support.ResponsePoliciesEvaluate;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmRestUtil;

/**
 * Implementation of authorization response interface to support OpenAM RESTful
 * authorization services.
 */
public class AuthorizationResponseImpl implements AuthorizationResponse {
    private final Status responseCode;
    private final List<ResponsePoliciesEvaluate> responseEntity;
    private final String responseUrl;
    private final Set<AuthorizationPolicyDecision> policyDecisions;

    /**
     * Constructs authorization response with specified parameters.
     * 
     * @param response
     *            JAX-RS response
     * @param resourceNames
     *            resource names
     * @param actionNames
     *            action names
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    public AuthorizationResponseImpl(Response response,
            Set<String> resourceNames, Set<String> actionNames)
            throws AuthXException {
        if (response == null)
            throw new AuthXException("Invalid authorization response <"
                    + response + ">.");

        OpenAmRestUtil.validateResources(resourceNames);

        responseUrl = null;

        responseCode = Status.fromStatusCode(response.getStatus());
        if (responseCode != Status.OK) {
            responseEntity = null;
            policyDecisions = null;

            return;
        }

        responseEntity = response
                .readEntity(new GenericType<List<ResponsePoliciesEvaluate>>() {
                });
        if ((responseEntity == null) || (responseEntity.size() < 1)) {
            policyDecisions = null;

            return;
        }

        policyDecisions = new HashSet<AuthorizationPolicyDecision>();

        for (ResponsePoliciesEvaluate i : responseEntity) {
            if (!resourceNames.contains(i.getResource()))
                continue;

            policyDecisions.add(new AuthorizationPolicyDecisionImpl(i,
                    actionNames));
        }
    }

    /**
     * Returns response code.
     * 
     * @return response code
     */
    @Override
    public Status getResponseCode() {
        return responseCode;
    }

    /**
     * Returns response URL. Currently unsupported by OpenAM.
     * 
     * @return response URL
     */
    @Override
    public String getResponseUrl() {
        throw new UnsupportedOperationException(
                OpenAmRestUtil.EXCEPTION_MSG_NO_SUPPORT);
    }

    /**
     * Returns authorization policy decisions.
     * 
     * @return authorization policy decisions
     */
    @Override
    public Set<AuthorizationPolicyDecision> getPolicyDecisions() {
        return policyDecisions;
    }

    /**
     * Returns underlying OpenAM policy evaluations.
     * 
     * @return underlying OpenAM policy evaluations
     */
    public List<ResponsePoliciesEvaluate> getResponseEntity() {
        return responseEntity;
    }

    /**
     * Returns string representation of authorization response.
     * 
     * @return string representation of authorization response
     */
    public String toString() {
        return "responseCode <" + responseCode + ">, policyDecisions <"
                + policyDecisions + ">";
    }
}
