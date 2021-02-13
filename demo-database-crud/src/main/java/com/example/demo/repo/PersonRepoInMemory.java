package com.example.demo.repo;

import java.util.LinkedList;
import java.util.List;

import org.springframework.stereotype.Repository;

import com.example.demo.model.PersonModel;

@Repository(PersonRepoInMemory.NAME)
public class PersonRepoInMemory implements PersonRepo {
	public static final String NAME = "personRepoInMemory";

	private static List<PersonModel> REPO = new LinkedList<>();

	@Override
	public List<PersonModel> post(PersonModel person) {
		REPO.add(person);
		return REPO;
	}

	@Override
	public List<PersonModel> get() {
		return REPO;
	}

	@Override
	public PersonModel get(String name) {
		for (PersonModel i : REPO)
			if (i.getName().equals(name))
				return i;

		return null;
	}

	@Override
	public PersonModel put(PersonModel person) {
		for (PersonModel i : REPO)
			if (i.getName().equals(person.getName())) {
				i.setAge(person.getAge());
				i.setGender(person.getGender());
				i.setState(person.getState());

				return i;
			}

		return null;
	}

	@Override
	public List<PersonModel> delete(String name) {
		for (int i = 0; i < REPO.size(); ++i)
			if (REPO.get(i).getName().equals(name)) {
				REPO.remove(i);
				break;
			}

		return REPO;
	}
}
