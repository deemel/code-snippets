package com.jupiter.mfdd.um.accesscontrol.testtool;

import java.io.IOException;

import org.codehaus.jackson.JsonParseException;
import org.codehaus.jackson.map.JsonMappingException;
import org.codehaus.jackson.map.ObjectMapper;
import org.kohsuke.args4j.Option;

import com.jupiter.mfdd.um.accesscontrol.testtool.authentication.DataAuthenticate;
import com.jupiter.mfdd.um.accesscontrol.testtool.authentication.DataDestroySession;
import com.jupiter.mfdd.um.accesscontrol.testtool.authentication.DataGetSessionToken;
import com.jupiter.mfdd.um.accesscontrol.testtool.authentication.DataIsValidSession;
import com.jupiter.mfdd.um.accesscontrol.testtool.authentication.DataRefreshSession;
import com.jupiter.mfdd.um.accesscontrol.testtool.authorization.DataAuthorize;
import com.jupiter.mfdd.um.accesscontrol.testtool.authorization.DataIsAuthorize;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataEncodeURL;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataGetAuthLevel;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataGetAuthType;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataGetHostName;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataGetIPAddress;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataGetIdleTime;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataGetMaxIdleTime;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataGetMaxSessionTime;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataGetPrincipal;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataGetProperty;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataGetSessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataGetTimeLeft;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataIsTokenRestricted;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken.DataSetProperty;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontokenid.DataEquals;
import com.jupiter.mfdd.um.accesscontrol.testtool.sessiontokenid.DataGetSessionTokenId;

public class Arguments {
    @Option(name = "--help", aliases = { "-h", "-?", "--?" }, usage = "print usage help", help = true)
    public boolean help;

    @Option(name = "--umServerUrl", usage = "UM server URL", metaVar = "<UM server URL>", required = true)
    public String umServerUrl;

    @Option(name = "--sessionTokenIdString", usage = "session token ID string", metaVar = "<session token ID string>")
    public String sessionTokenIdString;

    @Option(name = "--realm", usage = "realm", metaVar = "<realm>")
    public String realm;

    /*
     * SessionTokenID interface
     */
    @Option(name = "--getSessionTokenId", usage = "SessionTokenID.getSessionTokenId()")
    public boolean getSessionTokenId;

    @Option(name = "--equals", usage = "SessionTokenID.equals(Object object)")
    public boolean equals;

    public DataGetSessionTokenId dataGetSessionTokenId;
    public DataEquals dataEquals;

    /*
     * SessionToken interface
     */
    @Option(name = "--getSessionTokenID", usage = "SessionToken.getSessionTokenID()")
    public boolean getSessionTokenID;

    @Option(name = "--getMaxSessionTime", usage = "SessionToken.getMaxSessionTime()")
    public boolean getMaxSessionTime;

    @Option(name = "--getIdleTime", usage = "SessionToken.getIdleTime()")
    public boolean getIdleTime;

    @Option(name = "--getTimeLeft", usage = "SessionToken.getTimeLeft()")
    public boolean getTimeLeft;

    @Option(name = "--getMaxIdleTime", usage = "SessionToken.getMaxIdleTime()")
    public boolean getMaxIdleTime;

    @Option(name = "--getProperty", usage = "SessionToken.getProperty(String name)", metaVar = "'{\"name\":\"name1\"}'")
    public String getProperty;

    @Option(name = "--getPropertyLong", usage = "SessionToken.getProperty(String name, boolean ignoreState)", metaVar = "'{\"name\":\"name1\",\"ignoreState\":true}'")
    public String getPropertyLong;

    @Option(name = "--setProperty", usage = "SessionToken.setProperty(String name, String value)", metaVar = "'{\"name\":\"name1\",\"value\":\"value1\"}'")
    public String setProperty;

    @Option(name = "--isTokenRestricted", usage = "SessionToken.isTokenRestricted()")
    public boolean isTokenRestricted;

    @Option(name = "--encodeURL", usage = "SessionToken.encodeURL(String url)", metaVar = "'{\"url\":\"url1\"}'")
    public String encodeURL;

    @Option(name = "--getAuthLevel", usage = "SessionToken.getAuthLevel()")
    public boolean getAuthLevel;

    @Option(name = "--getAuthType", usage = "SessionToken.getAuthType()")
    public boolean getAuthType;

    @Option(name = "--getIPAddress", usage = "SessionToken.getIPAddress()")
    public boolean getIPAddress;

    @Option(name = "--getHostName", usage = "SessionToken.getHostName()")
    public boolean getHostName;

    @Option(name = "--getPrincipal", usage = "SessionToken.getPrincipal()")
    public boolean getPrincipal;

    public DataGetSessionTokenID dataGetSessionTokenID;
    public DataGetMaxSessionTime dataGetMaxSessionTime;
    public DataGetIdleTime dataGetIdleTime;
    public DataGetTimeLeft dataGetTimeLeft;
    public DataGetMaxIdleTime dataGetMaxIdleTime;
    public DataGetProperty dataGetProperty;
    public DataGetProperty dataGetPropertyLong;
    public DataSetProperty dataSetProperty;
    public DataIsTokenRestricted dataIsTokenRestricted;
    public DataEncodeURL dataEncodeURL;
    public DataGetAuthLevel dataGetAuthLevel;
    public DataGetAuthType dataGetAuthType;
    public DataGetIPAddress dataGetIPAddress;
    public DataGetHostName dataGetHostName;
    public DataGetPrincipal dataGetPrincipal;

    /*
     * AuthenticationService interface
     */
    @Option(name = "--getSessionToken", usage = "AuthenticationService.getSessionToken(SessionTokenID sessionTokenId)")
    public boolean getSessionToken;

    @Option(name = "--authenticate", usage = "AuthenticationService.authenticate(String realm, String userId, String password)", metaVar = "'{\"realm\":\"realm1\",\"userId\":\"userId1\",\"password\":\"password1\"}'")
    public String authenticate;

    @Option(name = "--refreshSession", usage = "AuthenticationService.refreshSession(SessionTokenID sessionTokenId)")
    public boolean refreshSession;

    @Option(name = "--destroySession", usage = "AuthenticationService.destroySession(SessionTokenID sessionTokenId)")
    public boolean destroySession;

    @Option(name = "--isValidSession", usage = "AuthenticationService.isValidSession(SessionTokenID sessionTokenId)")
    public boolean isValidSession;

    public DataGetSessionToken dataGetSessionToken;
    public DataAuthenticate dataAuthenticate;
    public DataRefreshSession dataRefreshSession;
    public DataDestroySession dataDestroySession;
    public DataIsValidSession dataIsValidSession;

    /*
     * AuthorizationService interface
     */
    @Option(name = "--isAuthorize", usage = "AuthorizationService.isAuthorized(String resourceName, String actionName)", metaVar = "'{\"resourceName\":\"resourceName1\",\"actionName\":\"actionName1\"}'")
    public String isAuthorize;

    @Option(name = "--isAuthorizeLong", usage = "AuthorizationService.isAuthorized(String resourceName, String actionName, Map<String, List<String>> envParameters)", metaVar = "'{\"resourceName\":\"resourceName1\",\"actionName\":\"actionName1\",\"envParameters\":{\"env1\":[\"value11\",\"value12\"],\"env2\":[\"value21\",\"value22\"]}}'")
    public String isAuthorizeLong;

    @Option(name = "--authorize", usage = "AuthorizationService.authorize(String resourceName, Set<String> actionNames, Map<String, List<String>> envParameters)", metaVar = "'{\"resourceName\":\"resourceName1\",\"actionNames\":[\"actionName1\",\"actionName2\"],\"envParameters\":{\"env1\":[\"value11\",\"value12\"],\"env2\":[\"value21\",\"value22\"]}}'")
    public String authorize;

    @Option(name = "--authorizeLong", usage = "AuthorizationService.authorize(Set<String> resourceNames, Set<String> actionNames, Map<String, List<String>> envParameters)", metaVar = "'{\"resourceNames\":[\"resourceName1\",\"resourceName2\"],\"actionNames\":[\"actionName1\",\"actionName2\"],\"envParameters\":{\"env1\":[\"value11\",\"value12\"],\"env2\":[\"value21\",\"value22\"]}}'")
    public String authorizeLong;

    public DataIsAuthorize dataIsAuthorize;
    public DataIsAuthorize dataIsAuthorizeLong;
    public DataAuthorize dataAuthorize;
    public DataAuthorize dataAuthorizeLong;

    public void parseOptions() throws JsonParseException, JsonMappingException,
            IOException {
        ObjectMapper mapper = new ObjectMapper();

        /*
         * SessionTokenID interface
         */
        if (getSessionTokenId)
            dataGetSessionTokenId = new DataGetSessionTokenId();

        if (equals)
            dataEquals = new DataEquals();

        /*
         * SessionToken interface
         */
        if (getSessionTokenID)
            dataGetSessionTokenID = new DataGetSessionTokenID();

        if (getMaxSessionTime)
            dataGetMaxSessionTime = new DataGetMaxSessionTime();

        if (getIdleTime)
            dataGetIdleTime = new DataGetIdleTime();

        if (getTimeLeft)
            dataGetTimeLeft = new DataGetTimeLeft();

        if (getMaxIdleTime)
            dataGetMaxIdleTime = new DataGetMaxIdleTime();

        if (getProperty != null)
            dataGetProperty = mapper.readValue(getProperty,
                    DataGetProperty.class);

        if (getPropertyLong != null)
            dataGetPropertyLong = mapper.readValue(getPropertyLong,
                    DataGetProperty.class);

        if (setProperty != null)
            dataSetProperty = mapper.readValue(setProperty,
                    DataSetProperty.class);

        if (isTokenRestricted)
            dataIsTokenRestricted = new DataIsTokenRestricted();

        if (encodeURL != null)
            dataEncodeURL = mapper.readValue(encodeURL, DataEncodeURL.class);

        if (getAuthLevel)
            dataGetAuthLevel = new DataGetAuthLevel();

        if (getAuthType)
            dataGetAuthType = new DataGetAuthType();

        if (getIPAddress)
            dataGetIPAddress = new DataGetIPAddress();

        if (getHostName)
            dataGetHostName = new DataGetHostName();

        if (getPrincipal)
            dataGetPrincipal = new DataGetPrincipal();

        /*
         * AuthenticationService interface
         */
        if (getSessionToken)
            dataGetSessionToken = new DataGetSessionToken();

        if (authenticate != null)
            dataAuthenticate = mapper.readValue(authenticate,
                    DataAuthenticate.class);

        if (refreshSession)
            dataRefreshSession = new DataRefreshSession();

        if (destroySession)
            dataDestroySession = new DataDestroySession();

        if (isValidSession)
            dataIsValidSession = new DataIsValidSession();

        /*
         * AuthorizationService interface
         */
        if (isAuthorize != null)
            dataIsAuthorize = mapper.readValue(isAuthorize,
                    DataIsAuthorize.class);

        if (isAuthorizeLong != null)
            dataIsAuthorizeLong = mapper.readValue(isAuthorizeLong,
                    DataIsAuthorize.class);

        if (authorize != null)
            dataAuthorize = mapper.readValue(authorize, DataAuthorize.class);

        if (authorizeLong != null)
            dataAuthorizeLong = mapper.readValue(authorizeLong,
                    DataAuthorize.class);
    }
}
