package com.example.demo.repo;

import java.util.HashSet;
import java.util.Set;

import org.springframework.stereotype.Repository;

import com.example.demo.model.FriendshipModel;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ArrayNode;

@Repository(FriendshipRepoTigerGraph.NAME)
public class FriendshipRepoTigerGraph implements FriendshipRepo {
	public static final String NAME = "friendshipRepoTigerGraph";

	@Override
	public Object post(FriendshipModel friendship) {
		return (TigerGraphUtil.post("/graph/social", friendship.toTigerGraphPayload()));
	}

	@Override
	public Object get() {
		// DML: How to easily retrieve all edges? Check out
		// https://community.tigergraph.com/t/what-is-the-gsql-for-selecting-all-verticies-and-edges-in-a-graph/203/9.

		JsonNode vertices = TigerGraphUtil.get("/graph/social/vertices/person");
		if (vertices.get("error").asBoolean())
			return (vertices.get("message").asText());

		Set<FriendshipModel> friendshipsSet = new HashSet<>();
		boolean isFirst = true;
		JsonNode edges = null;

		for (JsonNode i : (ArrayNode) vertices.get("results")) {
			String nameFrom = i.get("v_id").asText();
			JsonNode friendships = TigerGraphUtil.get("/graph/social/edges/person/" + nameFrom);

			if (isFirst) {
				edges = friendships;
				for (JsonNode j : (ArrayNode) edges.get("results"))
					friendshipsSet.add(new FriendshipModel(nameFrom, j.get("to_id").asText(), null));

				isFirst = false;
				continue;
			}

			for (JsonNode j : (ArrayNode) friendships.get("results")) {
				String nameTo = j.get("to_id").asText();
				if (friendshipsSet.contains(new FriendshipModel(nameTo, nameFrom, null)))
					continue;

				friendshipsSet.add(new FriendshipModel(nameFrom, nameTo, null));
				((ArrayNode) edges.get("results")).add(j);
			}
		}

		return edges;
	}

	@Override
	public Object get(String nameFrom) {
		return (TigerGraphUtil.get("/graph/social/edges/person/" + nameFrom));
	}

	@Override
	public Object get(String nameFrom, String nameTo) {
		return (TigerGraphUtil.get("/graph/social/edges/person/" + nameFrom + "/friendship/person/" + nameTo));
	}

	@Override
	public Object put(FriendshipModel friendship) {
		return (TigerGraphUtil.put("/graph/social", friendship.toTigerGraphPayload()));
	}

	@Override
	public Object delete(String nameFrom, String nameTo) {
		return (TigerGraphUtil.delete("/graph/social/edges/person/" + nameFrom + "/friendship/person/" + nameTo));
	}
}
