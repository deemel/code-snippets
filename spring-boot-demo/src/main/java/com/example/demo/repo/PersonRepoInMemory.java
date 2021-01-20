package com.example.demo.repo;

import java.util.LinkedList;
import java.util.List;
import java.util.UUID;

import org.springframework.stereotype.Repository;

import com.example.demo.model.PersonModel;

@Repository(PersonRepoInMemory.NAME)
public class PersonRepoInMemory implements PersonRepo {
	public static final String NAME = "repoInMemory";

	private static List<PersonModel> REPO = new LinkedList<>();

	@Override
	public List<PersonModel> post(PersonModel person) {
		if (person.getId() == null)
			person.setId(UUID.randomUUID());

		REPO.add(person);
		return REPO;
	}

	@Override
	public List<PersonModel> get() {
		return REPO;
	}

	@Override
	public PersonModel get(UUID id) {
		for (PersonModel i : REPO)
			if (i.getId().equals(id))
				return i;

		return null;
	}

	@Override
	public PersonModel put(PersonModel person) {
		for (PersonModel i : REPO)
			if (i.getId().equals(person.getId())) {
				i.setName(person.getName());
				return i;
			}

		return null;
	}

	@Override
	public PersonModel put(UUID id, PersonModel person) {
		for (PersonModel i : REPO)
			if (i.getId().equals(id)) {
				i.setId(person.getId());
				i.setName(person.getName());

				return i;
			}

		return null;
	}

	@Override
	public List<PersonModel> delete(UUID id) {
		for (int i = 0; i < REPO.size(); ++i)
			if (REPO.get(i).getId().equals(id)) {
				REPO.remove(i);
				break;
			}

		return REPO;
	}
}
