package com.jupiter.mfdd.um.accesscontrol.support;

import java.io.IOException;
import java.util.Set;

import javax.ws.rs.core.Response;
import javax.ws.rs.core.Response.Status;

import org.apache.commons.lang3.StringUtils;
import org.codehaus.jackson.JsonProcessingException;
import org.codehaus.jackson.map.ObjectMapper;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;

public class OpenAmRestUtil {
    public static final String PROP_UM_SERVER_URL = "um.server.url";
    public static final String EXCEPTION_MSG_NO_SUPPORT = "No apparent OpenAM REST support for this method.";

    /*
     * RESTEasy constants
     */
    public static final String PATH_PARAM_SESSION_TOKEN_ID = "sessionTokenId";
    public static final String PATH_PARAM_REALM = "realm";
    public static final String PATH_PARAM_SUBREALM = "subrealm";
    public static final String HEADER_PARAM_IPLANETDIRECTORYPRO = "iplanetDirectoryPro";
    public static final String HEADER_PARAM_USERNAME = "X-OpenAM-Username";
    public static final String HEADER_PARAM_PASSWORD = "X-OpenAM-Password";
    public static final String QUERY_PARAM_ACTION = "_action";
    public static final String QUERY_PARAM_REALM = "realm";
    public static final String QUERY_PARAM_REFRESH = "refresh";
    private static final String BRACES_PATH_PARAM_SESSION_TOKEN_ID = "{"
            + PATH_PARAM_SESSION_TOKEN_ID + "}";
    private static final String BRACES_PATH_PARAM_REALM = "{"
            + PATH_PARAM_REALM + "}";
    private static final String BRACES_PATH_PARAM_SUBREALM = "{"
            + PATH_PARAM_SUBREALM + "}";
    private static final String PATH_ROOT = "/openam/json";
    public static final String PATH_AUTHENTICATE = PATH_ROOT + "/authenticate";
    public static final String PATH_SESSIONS = PATH_ROOT + "/sessions";
    public static final String PATH_SESSIONS_VALIDATE = PATH_SESSIONS + "/"
            + BRACES_PATH_PARAM_SESSION_TOKEN_ID;
    public static final String PATH_POLICIES = PATH_ROOT + "/policies";
    public static final String PATH_POLICIES_REALM = PATH_ROOT + "/"
            + BRACES_PATH_PARAM_REALM + "/policies";
    public static final String PATH_POLICIES_SUBREALM = PATH_ROOT + "/"
            + BRACES_PATH_PARAM_REALM + "/" + BRACES_PATH_PARAM_SUBREALM
            + "/policies";
    public static final String PATH_USERS = PATH_ROOT + "/users";

    /*
     * Debug constants
     */
    private static final String SUBURL_ACTION = "/?" + QUERY_PARAM_ACTION + "=";
    public static final String URL_AUTHENTICATE = PATH_AUTHENTICATE + "?"
            + QUERY_PARAM_REALM + "=";
    public static final String URL_SESSIONS = PATH_SESSIONS + SUBURL_ACTION;
    public static final String URL_SESSIONS_VALIDATE = PATH_SESSIONS_VALIDATE
            + SUBURL_ACTION;
    public static final String URL_POLICIES = PATH_POLICIES + SUBURL_ACTION;
    public static final String URL_POLICIES_REALM = PATH_POLICIES_REALM
            + SUBURL_ACTION;
    public static final String URL_POLICIES_SUBREALM = PATH_POLICIES_SUBREALM
            + SUBURL_ACTION;
    public static final String URL_USERS = PATH_USERS + SUBURL_ACTION;

    private static String umServerUrl = null;

    public static String getUmServerUrl() throws AuthXException {
        if (umServerUrl != null)
            return umServerUrl;

        umServerUrl = System.getProperty(PROP_UM_SERVER_URL);
        if (StringUtils.isEmpty(umServerUrl))
            throw new AuthXException("Invalid UM server URL <" + umServerUrl
                    + ">.");

        return umServerUrl;
    }

    public static String getMethodName() {
        return Thread.currentThread().getStackTrace()[2].getMethodName();
    }

    public static void validateSessionTokenId(SessionTokenID sessionTokenId)
            throws AuthXException {
        if ((sessionTokenId == null)
                || (StringUtils.isEmpty(sessionTokenId.getSessionTokenId())))
            throw new AuthXException("Invalid session token ID <"
                    + sessionTokenId + ">.");
    }

    public static void validatePropertyName(String name) throws AuthXException {
        if (StringUtils.isEmpty(name))
            throw new AuthXException("Invalid property name <" + name + ">.");
    }

    public static void validateResources(Set<String> resources)
            throws AuthXException {
        if ((resources == null) || (resources.size() < 1))
            throw new AuthXException("Invalid resources <" + resources + ">.");
    }

    public static void validateUserId(String userId) throws AuthXException {
        if (StringUtils.isEmpty(userId))
            throw new AuthXException("Invalid user ID <" + userId + ">.");
    }

    public static void validatePassword(String password) throws AuthXException {
        if (password == null)
            throw new AuthXException("Invalid password <" + password + ">.");
    }

    public static void checkResponseStatus(Response response, String operation)
            throws AuthXException {
        int code = response.getStatus();
        if (code == Status.OK.getStatusCode())
            return;

        String responseString = response.readEntity(String.class);
        ResponseError responseError = new ResponseError(code,
                ("responseString <" + responseString + ">"));

        if (isValidJson(responseString))
            try {
                responseError = new ObjectMapper().readValue(responseString,
                        ResponseError.class);
            } catch (Exception e) {
            }

        throw new AuthXException("Operation <" + operation + "> failed; "
                + responseError + ".");
    }

    public static boolean isValidJson(String json) {
        try {
            new ObjectMapper().readTree(json);
            return true;
        } catch (JsonProcessingException e) {
            return false;
        } catch (IOException e) {
            return false;
        }
    }
}
