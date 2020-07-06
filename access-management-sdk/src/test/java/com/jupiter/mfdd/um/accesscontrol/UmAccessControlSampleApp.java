package com.jupiter.mfdd.um.accesscontrol;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import com.jupiter.boss.sdk.accesscontrol.SessionToken;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.boss.sdk.accesscontrol.authentication.AuthenticationResponse;
import com.jupiter.boss.sdk.accesscontrol.authentication.AuthenticationService;
import com.jupiter.boss.sdk.accesscontrol.authorization.AuthorizationResponse;
import com.jupiter.boss.sdk.accesscontrol.authorization.AuthorizationService;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactory;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmRestUtil;

public class UmAccessControlSampleApp {
    private static final String DEF_UM_SERVER_URL = "https://vmy0079-auth.ete.ka.sw.jupiter.se:19443";
    private static final String DEF_REALM = "/";
    private static final String DEF_USER_ID = "amAdmin";
    private static final String DEF_PASSWORD = "amAdmin123";
    private static final String URL_TO_ENCODE = "http://www.nba.com";
    private static final String PROP = "location";
    private static final String PROP_VAL = "piscataway";
    private static final String PROP_NONEXISTANT = "nonexistant_property";
    private static final String RESOURCE_NAME = "http://vmy0079-as.ete.ka.sw.jupiter.se:8080/oamrest/cm";
    private static final String ACTION_NAME = "POST";
    private static final String ENV_PARAM_KEY = "optional key1";
    private static final String ENV_PARAM_KEY2 = "optional key2";
    private static final String ENV_PARAM_VAL = "value";
    private static final String ENV_PARAM_VAL2 = "another value";

    private static String realm = DEF_REALM;
    private static String userId = DEF_USER_ID;
    private static String password = DEF_PASSWORD;

    private static void setup(String[] args) {
        if (System.getProperty(OpenAmRestUtil.PROP_UM_SERVER_URL) == null)
            System.setProperty(OpenAmRestUtil.PROP_UM_SERVER_URL,
                    DEF_UM_SERVER_URL);

        try {
            realm = args[0];
        } catch (ArrayIndexOutOfBoundsException e) {
        }

        try {
            userId = args[1];
        } catch (ArrayIndexOutOfBoundsException e) {
        }

        try {
            password = args[2];
        } catch (ArrayIndexOutOfBoundsException e) {
        }
    }

    private static void useSessionToken(AuthenticationService service) {
        System.out
                .println("================================================== "
                        + OpenAmRestUtil.getMethodName());
        System.out.println();

        AuthenticationResponse response = null;
        try {
            System.out.print(service.getClass().getSimpleName()
                    + ".authenticate: ");

            response = service.authenticate(realm, userId, password);

            System.out.println("responseCode <" + response.getResponseCode()
                    + ">, sessionTokenId <" + response.getSessionTokenID()
                    + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        SessionToken sessionToken = null;
        try {
            sessionToken = service
                    .getSessionToken(response.getSessionTokenID());
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println();
        }

        String className = sessionToken.getClass().getSimpleName();

        try {
            System.out.print(className + ".getSessionTokenID: ");
            System.out.println("sessionTokenID <"
                    + sessionToken.getSessionTokenID() + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getMaxSessionTime: ");
            System.out.println("rv <" + sessionToken.getMaxSessionTime() + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getTimeLeft: ");
            System.out.println("rv <" + sessionToken.getTimeLeft() + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getMaxIdleTime: ");
            System.out.println("rv <" + sessionToken.getMaxIdleTime() + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getIdleTime: ");
            System.out.println("rv <" + sessionToken.getIdleTime() + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".isTokenRestricted: ");
            System.out.println("rv <" + sessionToken.isTokenRestricted() + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".encodeURL: ");
            System.out.println("rv <" + sessionToken.encodeURL(URL_TO_ENCODE)
                    + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getAuthLevel: ");
            System.out.println("rv <" + sessionToken.getAuthLevel() + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getAuthType: ");
            System.out.println("rv <" + sessionToken.getAuthType() + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getHostName: ");
            System.out.println("rv <" + sessionToken.getHostName() + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getIPAddress: ");
            System.out.println("rv <" + sessionToken.getIPAddress() + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getPrincipal: ");
            System.out.println("rv <" + sessionToken.getPrincipal() + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".setProperty(" + PROP + ", "
                    + PROP_VAL + "): ");

            sessionToken.setProperty(PROP, PROP_VAL);

            System.out.println("rv <>");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getProperty(" + PROP + "): ");
            System.out.println("rv <" + sessionToken.getProperty(PROP) + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getProperty(" + PROP_NONEXISTANT
                    + "): ");
            System.out.println("rv <"
                    + sessionToken.getProperty(PROP_NONEXISTANT) + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getProperty(" + PROP + ", true): ");
            System.out.println("rv <" + sessionToken.getProperty(PROP, true)
                    + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".getProperty(" + PROP + ", false): ");
            System.out.println("rv <" + sessionToken.getProperty(PROP, false)
                    + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();
    }

    private static void useAuthenticationService(AuthenticationService service) {
        System.out
                .println("================================================== "
                        + OpenAmRestUtil.getMethodName());
        System.out.println();

        String className = service.getClass().getSimpleName();

        AuthenticationResponse response = null;
        try {
            System.out.print(className + ".authenticate: ");

            response = service.authenticate(realm, userId, password);

            System.out.println("responseCode <" + response.getResponseCode()
                    + ">, sessionTokenId <" + response.getSessionTokenID()
                    + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".isValidSession: ");
            System.out.println("rv <"
                    + service.isValidSession(response.getSessionTokenID())
                    + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".refreshSession: ");
            System.out.println("rv <"
                    + service.refreshSession(response.getSessionTokenID())
                    + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        SessionToken sessionToken = null;
        try {
            System.out.print(className + ".getSessionToken: ");

            sessionToken = service
                    .getSessionToken(response.getSessionTokenID());

            System.out.println("rv <" + sessionToken + ">");
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println();
        }
        System.out.println();

        try {
            System.out.print(sessionToken.getClass().getSimpleName()
                    + ".getSessionTokenID().equals("
                    + response.getClass().getSimpleName()
                    + ".getSessionTokenID()): ");
            System.out.println("rv <"
                    + sessionToken.getSessionTokenID().equals(
                            response.getSessionTokenID()) + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            System.out.print(className + ".destroySession: ");
            System.out.println("rv <"
                    + service.destroySession(response.getSessionTokenID())
                    + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();
    }

    private static void useAuthorizationService(AuthenticationService service,
            AuthorizationService serviceAuthorization) {
        System.out
                .println("================================================== "
                        + OpenAmRestUtil.getMethodName());
        System.out.println();

        AuthenticationResponse response = null;
        try {
            System.out.print(service.getClass().getSimpleName()
                    + ".authenticate: ");

            response = service.authenticate(realm, userId, password);

            System.out.println("responseCode <" + response.getResponseCode()
                    + ">, sessionTokenId <" + response.getSessionTokenID()
                    + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        SessionTokenID sessionTokenId = response.getSessionTokenID();
        String className = serviceAuthorization.getClass().getSimpleName();

        try {
            System.out.print(className + ".isAuthorized(" + sessionTokenId
                    + ", " + RESOURCE_NAME + ", " + ACTION_NAME + "): ");
            System.out.println("rv <"
                    + serviceAuthorization.isAuthorized(sessionTokenId,
                            RESOURCE_NAME, ACTION_NAME) + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            Map<String, List<String>> envParameters = new HashMap<String, List<String>>();
            envParameters.put(ENV_PARAM_KEY,
                    new LinkedList<>(Arrays.asList(ENV_PARAM_VAL)));

            System.out.print(className + ".isAuthorized(" + sessionTokenId
                    + ", " + RESOURCE_NAME + ", " + ACTION_NAME + ", "
                    + envParameters + "): ");
            System.out.println("rv <"
                    + serviceAuthorization.isAuthorized(sessionTokenId,
                            RESOURCE_NAME, ACTION_NAME, envParameters) + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            Set<String> actionNames = new HashSet<>(Arrays.asList(ACTION_NAME));

            Map<String, List<String>> envParameters = new HashMap<String, List<String>>();
            envParameters.put(ENV_PARAM_KEY,
                    new LinkedList<>(Arrays.asList(ENV_PARAM_VAL)));
            envParameters.put(ENV_PARAM_KEY2,
                    new LinkedList<>(Arrays.asList(ENV_PARAM_VAL)));

            System.out.print(className + ".authorize(" + sessionTokenId + ", "
                    + RESOURCE_NAME + ", " + actionNames + ", " + envParameters
                    + "): ");

            AuthorizationResponse authorizationResponse = serviceAuthorization
                    .authorize(sessionTokenId, RESOURCE_NAME, actionNames,
                            envParameters);

            System.out.println("rv <" + authorizationResponse + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();

        try {
            Set<String> resourceNames = new HashSet<>(
                    Arrays.asList(RESOURCE_NAME));
            Set<String> actionNames = new HashSet<>(Arrays.asList(ACTION_NAME));

            Map<String, List<String>> envParameters = new HashMap<String, List<String>>();
            envParameters.put(ENV_PARAM_KEY,
                    new LinkedList<>(Arrays.asList(ENV_PARAM_VAL)));
            envParameters.put(
                    ENV_PARAM_KEY2,
                    new LinkedList<>(Arrays.asList(ENV_PARAM_VAL,
                            ENV_PARAM_VAL2)));

            System.out.print(className + ".authorize(" + sessionTokenId + ", "
                    + resourceNames + ", " + actionNames + ", " + envParameters
                    + "): ");

            AuthorizationResponse authorizationResponse = serviceAuthorization
                    .authorize(sessionTokenId, resourceNames, actionNames,
                            envParameters);

            System.out.println("rv <" + authorizationResponse + ">");
        } catch (Exception e) {
            System.out.println();
            e.printStackTrace();
        }
        System.out.println();
    }

    public static void main(String[] args) {
        setup(args);

        AccessControlFactory accessControlFactory = new AccessControlFactoryImpl();

        try {
            useSessionToken(accessControlFactory.getAuthenticationService());
        } catch (Exception e) {
            e.printStackTrace();
        }

        try {
            useAuthenticationService(accessControlFactory
                    .getAuthenticationService());
        } catch (Exception e) {
            e.printStackTrace();
        }

        try {
            useAuthorizationService(
                    accessControlFactory.getAuthenticationService(),
                    accessControlFactory.getAuthorizationService());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
