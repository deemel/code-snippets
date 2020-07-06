package com.jupiter.mfdd.um.accesscontrol.support;

import javax.ws.rs.Consumes;
import javax.ws.rs.HeaderParam;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

public interface OpenAmRest {
    @Path(OpenAmRestUtil.PATH_AUTHENTICATE)
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    public Response authenticate(
            @HeaderParam(HttpHeaders.CONTENT_TYPE) String contentType,
            @HeaderParam(OpenAmRestUtil.HEADER_PARAM_USERNAME) String username,
            @HeaderParam(OpenAmRestUtil.HEADER_PARAM_PASSWORD) String password,
            @QueryParam(OpenAmRestUtil.QUERY_PARAM_REALM) String realm);

    @Path(OpenAmRestUtil.PATH_SESSIONS)
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    public Response sessions(
            @HeaderParam(HttpHeaders.CONTENT_TYPE) String contentType,
            @HeaderParam(OpenAmRestUtil.HEADER_PARAM_IPLANETDIRECTORYPRO) String sessionTokenId,
            @QueryParam(OpenAmRestUtil.QUERY_PARAM_ACTION) String action,
            @QueryParam(OpenAmRestUtil.QUERY_PARAM_REFRESH) String refresh,
            String data);

    @Path(OpenAmRestUtil.PATH_SESSIONS_VALIDATE)
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    public Response sessionsValidate(
            @PathParam(OpenAmRestUtil.PATH_PARAM_SESSION_TOKEN_ID) String sessionTokenId,
            @HeaderParam(HttpHeaders.CONTENT_TYPE) String contentType,
            @QueryParam(OpenAmRestUtil.QUERY_PARAM_ACTION) String action);

    @Path(OpenAmRestUtil.PATH_POLICIES)
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    public Response policies(
            @HeaderParam(HttpHeaders.CONTENT_TYPE) String contentType,
            @HeaderParam(OpenAmRestUtil.HEADER_PARAM_IPLANETDIRECTORYPRO) String sessionTokenId,
            @QueryParam(OpenAmRestUtil.QUERY_PARAM_ACTION) String action,
            String data);

    @Path(OpenAmRestUtil.PATH_POLICIES_REALM)
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    public Response policiesRealm(
            @PathParam(OpenAmRestUtil.PATH_PARAM_REALM) String realm,
            @HeaderParam(HttpHeaders.CONTENT_TYPE) String contentType,
            @HeaderParam(OpenAmRestUtil.HEADER_PARAM_IPLANETDIRECTORYPRO) String sessionTokenId,
            @QueryParam(OpenAmRestUtil.QUERY_PARAM_ACTION) String action,
            String data);

    @Path(OpenAmRestUtil.PATH_POLICIES_SUBREALM)
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    public Response policiesSubrealm(
            @PathParam(OpenAmRestUtil.PATH_PARAM_REALM) String realm,
            @PathParam(OpenAmRestUtil.PATH_PARAM_SUBREALM) String subrealm,
            @HeaderParam(HttpHeaders.CONTENT_TYPE) String contentType,
            @HeaderParam(OpenAmRestUtil.HEADER_PARAM_IPLANETDIRECTORYPRO) String sessionTokenId,
            @QueryParam(OpenAmRestUtil.QUERY_PARAM_ACTION) String action,
            String data);

    @Path(OpenAmRestUtil.PATH_USERS)
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    public Response users(
            @HeaderParam(HttpHeaders.CONTENT_TYPE) String contentType,
            @HeaderParam(OpenAmRestUtil.HEADER_PARAM_IPLANETDIRECTORYPRO) String sessionTokenId,
            @QueryParam(OpenAmRestUtil.QUERY_PARAM_ACTION) String action,
            String data);
}
