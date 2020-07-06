package com.jupiter.mfdd.um.accesscontrol.authentication;

import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import org.apache.commons.lang3.StringUtils;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.SessionToken;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.boss.sdk.accesscontrol.authentication.AuthenticationResponse;
import com.jupiter.boss.sdk.accesscontrol.authentication.AuthenticationService;
import com.jupiter.mfdd.um.accesscontrol.SessionTokenImpl;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.ResponseSessionsIsActive;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.ResponseSessionsLogout;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.ResponseSessionsValidate;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmHttpClient;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmRestUtil;
import com.jupiter.mfdd.um.accesscontrol.support.Realm;

/**
 * Implementation of authentication service interface to support OpenAM RESTful
 * authentication services.
 */
public class AuthenticationServiceImpl implements AuthenticationService {
    private static final String CLASS_NAME = AuthenticationServiceImpl.class
            .getSimpleName() + ".";

    /**
     * Returns session token associated with specified session token ID.
     * 
     * @param sessionTokenId
     *            session token ID
     * @return session token associated with specified session token ID
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public SessionToken getSessionToken(SessionTokenID sessionTokenId)
            throws AuthXException {
        OpenAmRestUtil.validateSessionTokenId(sessionTokenId);

        return new SessionTokenImpl(sessionTokenId);
    }

    /**
     * Returns authentication response upon successful authentication with
     * specified parameters.
     * 
     * @param realm
     *            realm; if empty, top level realm "/" is assumed
     * @param userId
     *            user ID
     * @param password
     *            password
     * @return authentication response upon successful authentication with
     *         specified parameters
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public AuthenticationResponse authenticate(String realm, String userId,
            String password) throws AuthXException {
        if (StringUtils.isEmpty(realm))
            realm = Realm.REALM_SEPARATOR;

        Realm.validate(realm);
        OpenAmRestUtil.validateUserId(userId);
        OpenAmRestUtil.validatePassword(password);

        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_AUTHENTICATE + ")";
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = openAmHttpClient.getOpenAmRestProxy()
                    .authenticate(MediaType.APPLICATION_JSON, userId, password,
                            realm);
            OpenAmRestUtil.checkResponseStatus(response, operation);

            AuthenticationResponse authenticationResponse = new AuthenticationResponseImpl(
                    response, new Realm(realm), userId, password);
            response.close();

            return authenticationResponse;
        } finally {
            openAmHttpClient.getClient().close();
        }
    }

    /**
     * Resets or refreshes a stateful session's idle time.
     * 
     * @param sessionTokenId
     *            session token ID
     * @return <code>true</code> if session remains active
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public boolean refreshSession(SessionTokenID sessionTokenId)
            throws AuthXException {
        return isActiveSession(sessionTokenId, true);
    }

    /**
     * Logs out or destroys a previously authenticated session.
     * 
     * @param sessionTokenId
     *            session token ID
     * @return <code>true</code> if specified session has been successfully
     *         logged out
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public boolean destroySession(SessionTokenID sessionTokenId)
            throws AuthXException {
        OpenAmRestUtil.validateSessionTokenId(sessionTokenId);

        String action = "logout";
        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_SESSIONS + action + ")";
        String sessionTokenIdString = sessionTokenId.getSessionTokenId();
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = openAmHttpClient.getOpenAmRestProxy().sessions(
                    MediaType.APPLICATION_JSON, sessionTokenIdString, action,
                    "", "");
            OpenAmRestUtil.checkResponseStatus(response, operation);

            ResponseSessionsLogout responseEntity = response
                    .readEntity(ResponseSessionsLogout.class);
            response.close();

            return responseEntity.isSuccess();
        } finally {
            openAmHttpClient.getClient().close();
        }
    }

    /**
     * Returns <code>true</code> if specified session is valid. Validation has
     * tangential effect of resetting a stateful session's idle time.
     * 
     * @param sessionTokenId
     *            session token ID
     * @return <code>true</code> if specified session is valid
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public boolean isValidSession(SessionTokenID sessionTokenId)
            throws AuthXException {
        OpenAmRestUtil.validateSessionTokenId(sessionTokenId);

        String action = "validate";
        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_SESSIONS_VALIDATE + action + ")";
        String sessionTokenIdString = sessionTokenId.getSessionTokenId();
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = openAmHttpClient.getOpenAmRestProxy()
                    .sessionsValidate(sessionTokenIdString,
                            MediaType.APPLICATION_JSON, action);
            OpenAmRestUtil.checkResponseStatus(response, operation);

            ResponseSessionsValidate responseEntity = response
                    .readEntity(ResponseSessionsValidate.class);
            response.close();

            return responseEntity.getValid();
        } finally {
            openAmHttpClient.getClient().close();
        }
    }

    /**
     * Returns <code>true</code> if specified session is active.
     * 
     * @param sessionTokenId
     *            session token ID
     * @return <code>true</code> if specified session is active
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    public boolean isActiveSession(SessionTokenID sessionTokenId)
            throws AuthXException {
        return isActiveSession(sessionTokenId, false);
    }

    /**
     * Returns <code>true</code> if specified session is active.
     * 
     * @param sessionTokenId
     *            session token ID
     * @param isRefresh
     *            <code>true</code> if stateful session's idle time is to be
     *            refreshed
     * @return <code>true</code> if specified session is active
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    public boolean isActiveSession(SessionTokenID sessionTokenId,
            boolean isRefresh) throws AuthXException {
        OpenAmRestUtil.validateSessionTokenId(sessionTokenId);

        String action = "isActive";
        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_SESSIONS + action + ")";
        String sessionTokenIdString = sessionTokenId.getSessionTokenId();
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = openAmHttpClient.getOpenAmRestProxy().sessions(
                    MediaType.APPLICATION_JSON, sessionTokenIdString, action,
                    Boolean.toString(isRefresh), "");
            OpenAmRestUtil.checkResponseStatus(response, operation);

            ResponseSessionsIsActive responseEntity = response
                    .readEntity(ResponseSessionsIsActive.class);
            response.close();

            return responseEntity.getActive();
        } finally {
            openAmHttpClient.getClient().close();
        }
    }
}
