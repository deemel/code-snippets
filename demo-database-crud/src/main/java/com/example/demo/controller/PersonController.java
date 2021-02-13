package com.example.demo.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import com.example.demo.model.PersonModel;
import com.example.demo.service.PersonService;

@RestController
@RequestMapping("api/v1/person")
public class PersonController {
	private final PersonService personService;

	@Autowired
	public PersonController(PersonService personService) {
		this.personService = personService;
	}

	@PostMapping
	public Object post(@RequestBody PersonModel person) {
		return personService.post(person);
	}

	@GetMapping
	public Object get(@RequestParam(name = "name", required = false) String name) {
		return ((name == null) ? (personService.get()) : (personService.get(name)));
	}

	@PutMapping
	public Object put(@RequestBody PersonModel person) {
		return personService.put(person);
	}

	@DeleteMapping
	public Object delete(@RequestParam("name") String name) {
		return personService.delete(name);
	}
}
