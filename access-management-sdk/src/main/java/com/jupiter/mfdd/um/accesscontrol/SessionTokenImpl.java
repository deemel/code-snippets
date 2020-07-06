package com.jupiter.mfdd.um.accesscontrol;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.security.Principal;
import java.util.Map;

import javax.ws.rs.core.GenericType;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import org.apache.commons.lang3.StringUtils;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.SessionToken;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.DataSessionsGetProperty;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.DataSessionsSetProperty;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.PrincipalImpl;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.ResponseSessionsGetIdle;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.ResponseSessionsGetMaxIdle;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.ResponseSessionsGetMaxSessionTime;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.ResponseSessionsGetProperty;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.ResponseSessionsGetTimeLeft;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.ResponseSessionsSetProperty;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.ResponseUsersIdFromSession;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmHttpClient;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmRestUtil;

/**
 * Implementation of session token interface to support OpenAM RESTful services.
 */
public class SessionTokenImpl implements SessionToken {
    private static final String CLASS_NAME = SessionTokenImpl.class
            .getSimpleName() + ".";

    private final SessionTokenID sessionTokenId;
    private final String sessionTokenIdString;

    /**
     * Constructs session token with specified parameters.
     * 
     * @param sessionTokenId
     *            session token ID
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    public SessionTokenImpl(SessionTokenID sessionTokenId)
            throws AuthXException {
        OpenAmRestUtil.validateSessionTokenId(sessionTokenId);

        this.sessionTokenId = sessionTokenId;
        sessionTokenIdString = this.sessionTokenId.getSessionTokenId();
    }

    /**
     * Returns session token ID.
     * 
     * @return session token ID
     */
    @Override
    public SessionTokenID getSessionTokenID() {
        return sessionTokenId;
    }

    /**
     * Returns configured maximum session time (in minutes).
     * 
     * @return configured maximum session time (in minutes)
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public long getMaxSessionTime() throws AuthXException {
        String action = "getMaxSessionTime";
        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_SESSIONS + action + ")";
        String sessionTokenIdString = sessionTokenId.getSessionTokenId();
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = openAmHttpClient.getOpenAmRestProxy().sessions(
                    MediaType.APPLICATION_JSON, sessionTokenIdString, action,
                    "", "");
            OpenAmRestUtil.checkResponseStatus(response, operation);

            ResponseSessionsGetMaxSessionTime responseEntity = response
                    .readEntity(ResponseSessionsGetMaxSessionTime.class);
            response.close();

            return responseEntity.getMaxsessiontime();
        } finally {
            openAmHttpClient.getClient().close();
        }
    }

    /**
     * Returns amount of time (in seconds) that a stateful session has been
     * idle.
     * 
     * @return amount of time (in seconds) that a stateful session has been idle
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public long getIdleTime() throws AuthXException {
        String action = "getIdle";
        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_SESSIONS + action + ")";
        String sessionTokenIdString = sessionTokenId.getSessionTokenId();
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = openAmHttpClient.getOpenAmRestProxy().sessions(
                    MediaType.APPLICATION_JSON, sessionTokenIdString, action,
                    "", "");
            OpenAmRestUtil.checkResponseStatus(response, operation);

            ResponseSessionsGetIdle responseEntity = response
                    .readEntity(ResponseSessionsGetIdle.class);
            response.close();

            return responseEntity.getIdletime();
        } finally {
            openAmHttpClient.getClient().close();
        }
    }

    /**
     * Returns session's maximum remaining time (in seconds).
     * 
     * @return session's maximum remaining time (in seconds)
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public long getTimeLeft() throws AuthXException {
        String action = "getTimeLeft";
        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_SESSIONS + action + ")";
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = openAmHttpClient.getOpenAmRestProxy().sessions(
                    MediaType.APPLICATION_JSON, sessionTokenIdString, action,
                    "", "");
            OpenAmRestUtil.checkResponseStatus(response, operation);

            ResponseSessionsGetTimeLeft responseEntity = response
                    .readEntity(ResponseSessionsGetTimeLeft.class);
            response.close();

            return responseEntity.getMaxtime();
        } finally {
            openAmHttpClient.getClient().close();
        }
    }

    /**
     * Returns configured maximum idle time (in minutes).
     * 
     * @return configured maximum idle time (in minutes)
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public long getMaxIdleTime() throws AuthXException {
        String action = "getMaxIdle";
        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_SESSIONS + action + ")";
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = openAmHttpClient.getOpenAmRestProxy().sessions(
                    MediaType.APPLICATION_JSON, sessionTokenIdString, action,
                    "", "");
            OpenAmRestUtil.checkResponseStatus(response, operation);

            ResponseSessionsGetMaxIdle responseEntity = response
                    .readEntity(ResponseSessionsGetMaxIdle.class);
            response.close();

            return responseEntity.getMaxidletime();
        } finally {
            openAmHttpClient.getClient().close();
        }
    }

    /**
     * Returns specified property.
     * 
     * @param name
     *            property name
     * @return specified property
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public String getProperty(String name) throws AuthXException {
        OpenAmRestUtil.validatePropertyName(name);

        String action = "getProperty";
        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_SESSIONS + action + ")";
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = null;
            try {
                DataSessionsGetProperty data = new DataSessionsGetProperty(name);

                response = openAmHttpClient.getOpenAmRestProxy().sessions(
                        MediaType.APPLICATION_JSON, sessionTokenIdString,
                        action, "", data.toJson());
            } catch (Exception e) {
                throw new AuthXException(e);
            }
            OpenAmRestUtil.checkResponseStatus(response, operation);

            ResponseSessionsGetProperty responseEntity = new ResponseSessionsGetProperty(
                    response.readEntity(new GenericType<Map<String, String>>() {
                    }));
            response.close();

            return responseEntity.get(name);
        } finally {
            openAmHttpClient.getClient().close();
        }
    }

    /**
     * Returns specified property.
     * 
     * @param name
     *            property name
     * @param ignoreState
     *            <code>true</code> if session token ID's state is to be
     *            ignored; currently unsupported by OpenAM
     * @return specified property
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public String getProperty(String name, boolean ignoreState)
            throws AuthXException {
        return getProperty(name);
    }

    /**
     * Sets specified property.
     * 
     * @param name
     *            property name
     * @param value
     *            property value
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public void setProperty(String name, String value) throws AuthXException {
        OpenAmRestUtil.validatePropertyName(name);

        String action = "setProperty";
        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_SESSIONS + action + ")";
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = null;
            try {
                DataSessionsSetProperty data = new DataSessionsSetProperty(
                        name, value);

                response = openAmHttpClient.getOpenAmRestProxy().sessions(
                        MediaType.APPLICATION_JSON, sessionTokenIdString,
                        action, "", data.toJson());
            } catch (Exception e) {
                throw new AuthXException(e);
            }
            OpenAmRestUtil.checkResponseStatus(response, operation);

            ResponseSessionsSetProperty responseEntity = response
                    .readEntity(ResponseSessionsSetProperty.class);
            response.close();

            if (!responseEntity.getSuccess())
                throw new AuthXException("Operation <" + operation
                        + "> failed; success <" + responseEntity.getSuccess()
                        + ">.");
        } finally {
            openAmHttpClient.getClient().close();
        }
    }

    /**
     * Returns <code>true</code> if session token is restricted. Currently
     * unsupported by OpenAM.
     * 
     * @return <code>true</code> if session token is restricted
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public boolean isTokenRestricted() throws AuthXException {
        throw new UnsupportedOperationException(
                OpenAmRestUtil.EXCEPTION_MSG_NO_SUPPORT);
    }

    /**
     * Returns encoded URL, rewritten to include session token ID. Currently
     * unsupported by OpenAM.
     * 
     * @param url
     *            URL to be encoded
     * @return encoded URL, rewritten to include session token ID
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public String encodeURL(String url) throws AuthXException {
        if (StringUtils.isEmpty(url))
            throw new AuthXException("Invalid URL <" + url + ">.");

        throw new UnsupportedOperationException(
                OpenAmRestUtil.EXCEPTION_MSG_NO_SUPPORT);
    }

    /*
     * From OpenAM Developer's Guide, Version 13, 2.1.1.8 Token Validation and
     * Session Information, last paragraph in section: "You can not read or set
     * properties internal to OpenAM sessions. If you specify an internal
     * property in a REST API call, a 403 Forbidden error is returned."
     */
    /**
     * Returns authentication level used for authentication. Currently
     * unsupported by OpenAM.
     * 
     * @return authentication level used for authentication
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public int getAuthLevel() throws AuthXException {
        return Integer.valueOf(getProperty("AuthLevel"));
    }

    /**
     * Returns authentication type or method used for authentication. Currently
     * unsupported by OpenAM.
     * 
     * @return authentication type or method used for authentication
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public String getAuthType() throws AuthXException {
        return getProperty("AuthType");
    }

    /**
     * Returns IP address of client which sent request. Currently unsupported by
     * OpenAM.
     * 
     * @return IP address of client which sent request
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public InetAddress getIPAddress() throws AuthXException {
        try {
            return InetAddress.getByName(getProperty("Host"));
        } catch (UnknownHostException e) {
            throw new AuthXException(e);
        }
    }

    /**
     * Returns hostname of client which sent request. Currently unsupported by
     * OpenAM.
     * 
     * @return hostname of client which sent request
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public String getHostName() throws AuthXException {
        return getProperty("HostName");
    }

    /**
     * Returns principal.
     * 
     * @return principal
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    @Override
    public Principal getPrincipal() throws AuthXException {
        // return new PrincipalImpl(getProperty("Principal"));

        String action = "idFromSession";
        String operation = CLASS_NAME + OpenAmRestUtil.getMethodName() + " ("
                + OpenAmRestUtil.URL_USERS + action + ")";
        String sessionTokenIdString = sessionTokenId.getSessionTokenId();
        OpenAmHttpClient openAmHttpClient = new OpenAmHttpClient();

        try {
            Response response = openAmHttpClient.getOpenAmRestProxy().users(
                    MediaType.APPLICATION_JSON, sessionTokenIdString, action,
                    "");
            OpenAmRestUtil.checkResponseStatus(response, operation);

            ResponseUsersIdFromSession responseEntity = response
                    .readEntity(ResponseUsersIdFromSession.class);
            response.close();

            return new PrincipalImpl(responseEntity);
        } finally {
            openAmHttpClient.getClient().close();
        }
    }

    /**
     * Returns session token ID string.
     * 
     * @return session token ID string
     */
    public String toString() {
        return sessionTokenId.toString();
    }
}
