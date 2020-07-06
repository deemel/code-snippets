package com.jupiter.mfdd.um.accesscontrol.authentication.support;

import java.security.Principal;

import org.apache.commons.lang3.StringUtils;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;

public class PrincipalImpl implements Principal {
    private String name;
    private ResponseUsersIdFromSession response;

    /**
     * Constructs principal with specified parameters.
     * 
     * @param response
     *            OpenAM user ID from session response
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    public PrincipalImpl(ResponseUsersIdFromSession response)
            throws AuthXException {
        if (response == null)
            throw new AuthXException(
                    "Invalid OpenAM user ID from session response <" + response
                            + ">.");

        this.response = response;
    }

    /**
     * Constructs principal with specified parameters.
     * 
     * @param name
     *            principal name
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    public PrincipalImpl(String name) throws AuthXException {
        if (StringUtils.isEmpty(name))
            throw new AuthXException("Invalid principal name <" + name + ">.");

        this.name = name;
    }

    /**
     * Returns principal name or distinguished name (DN).
     * 
     * @return principal name or distinguished name (DN)
     */
    @Override
    public String getName() {
        return ((name != null) ? (name) : (response.getDn()));
    }

    /**
     * Returns underlying OpenAM user ID from session response.
     * 
     * @return underlying OpenAM user ID from session response
     */
    public ResponseUsersIdFromSession getResponse() {
        return response;
    }

    /**
     * Returns string representation of principal.
     * 
     * @return string representation of principal
     */
    public String toString() {
        return "name <" + getName() + ">, response <" + response + ">";
    }
}
