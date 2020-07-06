package com.jupiter.mfdd.um.accesscontrol.authorization;

import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import org.apache.commons.lang3.StringUtils;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.boss.sdk.accesscontrol.authorization.AuthorizationResponse;
import com.jupiter.boss.sdk.accesscontrol.authorization.AuthorizationService;
import com.jupiter.mfdd.um.accesscontrol.SessionTokenIDImpl;
import com.jupiter.mfdd.um.accesscontrol.authorization.support.DataPoliciesEvaluate;
import com.jupiter.mfdd.um.accesscontrol.authorization.support.ResponsePoliciesEvaluate;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmHttpClient;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmRestUtil;

/**
 * Implementation of authorization service interface to support OpenAM RESTful
 * authorization services.
 */
public class AuthorizationServiceImpl implements AuthorizationService {
    private static final String CLASS_NAME = AuthorizationServiceImpl.class
            .getSimpleName() + ".";

    /**
     * Returns <code>true</code> if authorization is successful for specified
     * parameters.
     * 
     * @param sessionTokenId
     *            session token ID
     * @param resourceName
     *            resource name
     * @param actionName
     *            action name
     * @return <code>true</code> if authorization is successful for specified
     *         parameters
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public boolean isAuthorized(SessionTokenID sessionTokenId,
            String resourceName, String actionName) throws AuthXException {
        return isAuthorized(sessionTokenId, resourceName, actionName, null);
    }

    /**
     * Returns <code>true</code> if authorization is successful for specified
     * parameters.
     * 
     * @param sessionTokenId
     *            session token ID
     * @param resourceName
     *            resource name
     * @param actionName
     *            action name
     * @param envParameters
     *            optional environment parameters
     * @return <code>true</code> if authorization is successful for specified
     *         parameters
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public boolean isAuthorized(SessionTokenID sessionTokenId,
            String resourceName, String actionName,
            Map<String, List<String>> envParameters) throws AuthXException {
        if (!(sessionTokenId instanceof SessionTokenIDImpl))
            throw new AuthXException("Invalid session token ID <"
                    + sessionTokenId + ">; not instance of <"
                    + SessionTokenIDImpl.class.getSimpleName() + ">.");

        SessionTokenIDImpl sessionTokenIdImpl = (SessionTokenIDImpl) sessionTokenId;

        OpenAmRestUtil.validateSessionTokenId(sessionTokenIdImpl);

        if (StringUtils.isEmpty(resourceName))
            throw new AuthXException("Invalid resource name <" + resourceName
                    + ">.");

        if (StringUtils.isEmpty(actionName))
            throw new AuthXException("Invalid action name <" + actionName
                    + ">.");

        String action = "evaluate";
        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_POLICIES_REALM + action + ")";
        String sessionTokenIdString = sessionTokenIdImpl.getSessionTokenId();
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = null;
            try {
                String partRealm = sessionTokenIdImpl.getRealm().getPartRealm();
                String partSubrealm = sessionTokenIdImpl.getRealm()
                        .getPartSubrealm();
                DataPoliciesEvaluate data = new DataPoliciesEvaluate(
                        new HashSet<>(Arrays.asList(resourceName)),
                        envParameters);

                if (StringUtils.isEmpty(partRealm)) {
                    response = openAmHttpClient.getOpenAmRestProxy().policies(
                            MediaType.APPLICATION_JSON, sessionTokenIdString,
                            action, data.toJson());
                } else {
                    if (StringUtils.isEmpty(partSubrealm)) {
                        response = openAmHttpClient.getOpenAmRestProxy()
                                .policiesRealm(partRealm,
                                        MediaType.APPLICATION_JSON,
                                        sessionTokenIdString, action,
                                        data.toJson());
                    } else {
                        response = openAmHttpClient.getOpenAmRestProxy()
                                .policiesSubrealm(partRealm, partSubrealm,
                                        MediaType.APPLICATION_JSON,
                                        sessionTokenIdString, action,
                                        data.toJson());
                    }
                }
            } catch (Exception e) {
                throw new AuthXException(e);
            }
            OpenAmRestUtil.checkResponseStatus(response, operation);

            boolean isAuthorized = ResponsePoliciesEvaluate.isAuthorized(
                    response, resourceName, actionName);
            response.close();

            return isAuthorized;
        } finally {
            openAmHttpClient.getClient().close();
        }
    }

    /**
     * Returns authorization response containing authorization policy decisions
     * for specified parameters.
     * 
     * @param sessionTokenId
     *            session token ID
     * @param resourceName
     *            required resource name
     * @param actionNames
     *            optional action names; if undefined, all allowable actions are
     *            returned
     * @param envParameters
     *            optional conditional environment parameters
     * @return authorization response containing authorization policy decisions
     *         for specified parameters
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public AuthorizationResponse authorize(SessionTokenID sessionTokenId,
            String resourceName, Set<String> actionNames,
            Map<String, List<String>> envParameters) throws AuthXException {
        return authorize(sessionTokenId,
                new HashSet<>(Arrays.asList(resourceName)), actionNames,
                envParameters);
    }

    /**
     * Returns authorization response containing authorization policy decisions
     * for specified parameters.
     * 
     * @param sessionTokenId
     *            session token ID
     * @param resourceNames
     *            required resource names
     * @param actionNames
     *            optional action names; if undefined, all allowable actions are
     *            returned
     * @param envParameters
     *            optional conditional environment parameters
     * @return authorization response containing authorization policy decisions
     *         for specified parameters
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public AuthorizationResponse authorize(SessionTokenID sessionTokenId,
            Set<String> resourceNames, Set<String> actionNames,
            Map<String, List<String>> envParameters) throws AuthXException {
        if (!(sessionTokenId instanceof SessionTokenIDImpl))
            throw new AuthXException("Invalid session token ID <"
                    + sessionTokenId + ">; not instance of <"
                    + SessionTokenIDImpl.class.getSimpleName() + ">.");

        SessionTokenIDImpl sessionTokenIdImpl = (SessionTokenIDImpl) sessionTokenId;

        OpenAmRestUtil.validateSessionTokenId(sessionTokenIdImpl);
        OpenAmRestUtil.validateResources(resourceNames);

        String action = "evaluate";
        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_POLICIES_REALM + action + ")";
        String sessionTokenIdString = sessionTokenIdImpl.getSessionTokenId();
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = null;
            try {
                String partRealm = sessionTokenIdImpl.getRealm().getPartRealm();
                String partSubrealm = sessionTokenIdImpl.getRealm()
                        .getPartSubrealm();
                DataPoliciesEvaluate data = new DataPoliciesEvaluate(
                        resourceNames, envParameters);

                if (StringUtils.isEmpty(partRealm)) {
                    response = openAmHttpClient.getOpenAmRestProxy().policies(
                            MediaType.APPLICATION_JSON, sessionTokenIdString,
                            action, data.toJson());
                } else {
                    if (StringUtils.isEmpty(partSubrealm)) {
                        response = openAmHttpClient.getOpenAmRestProxy()
                                .policiesRealm(partRealm,
                                        MediaType.APPLICATION_JSON,
                                        sessionTokenIdString, action,
                                        data.toJson());
                    } else {
                        response = openAmHttpClient.getOpenAmRestProxy()
                                .policiesSubrealm(partRealm, partSubrealm,
                                        MediaType.APPLICATION_JSON,
                                        sessionTokenIdString, action,
                                        data.toJson());
                    }
                }
            } catch (Exception e) {
                throw new AuthXException(e);
            }
            OpenAmRestUtil.checkResponseStatus(response, operation);

            AuthorizationResponse authorizationResponse = new AuthorizationResponseImpl(
                    response, resourceNames, actionNames);
            response.close();

            return authorizationResponse;
        } finally {
            openAmHttpClient.getClient().close();
        }
    }
}
