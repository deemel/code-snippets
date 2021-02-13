package com.example.demo.repo;

import com.example.demo.model.PersonModel;

public interface PersonRepo {
	public Object post(PersonModel person);

	public Object get();

	public Object get(String name);

	public Object put(PersonModel person);

	public Object delete(String name);
}
