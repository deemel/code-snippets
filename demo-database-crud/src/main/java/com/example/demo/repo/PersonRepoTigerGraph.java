package com.example.demo.repo;

import org.springframework.stereotype.Repository;

import com.example.demo.model.PersonModel;

@Repository(PersonRepoTigerGraph.NAME)
public class PersonRepoTigerGraph implements PersonRepo {
	public static final String NAME = "personRepoTigerGraph";

	@Override
	public Object post(PersonModel person) {
		return (TigerGraphUtil.post("/graph/social", person.toTigerGraphPayload()));
	}

	@Override
	public Object get() {
		return (TigerGraphUtil.get("/graph/social/vertices/person"));
	}

	@Override
	public Object get(String name) {
		return (TigerGraphUtil.get("/graph/social/vertices/person/" + name));
	}

	@Override
	public Object put(PersonModel person) {
		return (TigerGraphUtil.put("/graph/social", person.toTigerGraphPayload()));
	}

	@Override
	public Object delete(String name) {
		return (TigerGraphUtil.delete("/graph/social/vertices/person/" + name));
	}
}
