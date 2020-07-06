package com.jupiter.mfdd.um.accesscontrol.authentication;

import javax.ws.rs.core.Response;
import javax.ws.rs.core.Response.Status;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.boss.sdk.accesscontrol.authentication.AuthenticationResponse;
import com.jupiter.mfdd.um.accesscontrol.SessionTokenIDImpl;
import com.jupiter.mfdd.um.accesscontrol.authentication.support.ResponseAuthenticate;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmRestUtil;
import com.jupiter.mfdd.um.accesscontrol.support.Realm;

/**
 * Implementation of authentication response interface to support OpenAM RESTful
 * authentication services.
 */
public class AuthenticationResponseImpl implements AuthenticationResponse {
    private final Status responseCode;
    private final ResponseAuthenticate responseEntity;
    private final SessionTokenID sessionTokenId;
    private final String responseUrl;

    /**
     * Constructs authentication response with specified parameters.
     * 
     * @param response
     *            JAX-RS response
     * @param realm
     *            realm
     * @param userId
     *            user ID
     * @param password
     *            password
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    public AuthenticationResponseImpl(Response response, Realm realm,
            String userId, String password) throws AuthXException {
        if (response == null)
            throw new AuthXException("Invalid authentication response <"
                    + response + ">.");

        responseUrl = null;

        responseCode = Status.fromStatusCode(response.getStatus());
        if (responseCode != Status.OK) {
            responseEntity = null;
            sessionTokenId = null;

            return;
        }

        responseEntity = response.readEntity(ResponseAuthenticate.class);
        sessionTokenId = new SessionTokenIDImpl(responseEntity.getTokenId(),
                realm, userId, password);
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
     * Returns session token ID.
     * 
     * @return session token ID
     */
    @Override
    public SessionTokenID getSessionTokenID() {
        return sessionTokenId;
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
     * Returns underlying OpenAM response.
     * 
     * @return underlying OpenAM response
     */
    public ResponseAuthenticate getResponseEntity() {
        return responseEntity;
    }
}
