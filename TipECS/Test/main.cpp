#include "TipECS/TMPLib.h"
#include "TipECS/Registry.h"
#include "TipECS/EntityManager.h"
#include "TipECS/Components.h"

#include <iostream>
#include <tuple>
#include <type_traits>

struct A { int a; A() = default; A(int i) : a(i) {} };
struct B { char c; double b; B() = default; B(char c, double d) : c(c), b(d) {} };
struct C { const char* text = "hello!"; C() = default; C(const char* t) :text(t) {} };
struct D { int a, b, c, d; };

struct TagA {};
struct TagB {};

void TestTMPLib()
{
	if constexpr (TMP::TypeList<A, B>::Contain<A>())
		std::cout << "Contain A!\n";
	else
		std::cout << "Not Contain A!\n";

	if constexpr (TMP::TypeList<A, B>::Contain<B>())
		std::cout << "Contain B!\n";
	else
		std::cout << "Not Contain B!\n";

	if constexpr (TMP::TypeList<A, B>::Contain<C>())
		std::cout << "Contain C!\n";
	else
		std::cout << "Not Contain C!\n";
}

using MyCompList = TipECS::CompoentList<A, B, C>;
using MyTagList = TipECS::TagList<TagA, TagB>;

using SignatureA = TipECS::Signature<A, B, TagA>;
using SignatureB = TipECS::Signature<A, TagB, B, TagA>;
using SignatureC = TipECS::Signature<A, C>;

using MySignatureList = TipECS::SignatureList<SignatureA, SignatureB, SignatureC>;

using MySetting = TipECS::Setting<MyCompList, MyTagList, MySignatureList>;

void TestECSRegistry()
{
	static_assert(MySetting::IsComponent<A>(), "A is not a component!");
	static_assert(MySetting::IsComponent<B>(), "B is not a component!");
	static_assert(MySetting::IsComponent<C>(), "C is not a component!");
	//static_assert(MySettings::IsComponent<D>(), "D is not a component!");

	static_assert(MySetting::IsTag<TagA>(), "TagA is not a tag!");
	static_assert(MySetting::IsTag<TagB>(), "TagB is not a tag!");
	//static_assert(MySettings::IsTag<C>(), "C is not a tag!");

	static_assert(MySetting::IsSignature<SignatureA>(), "SignatureA is not a signature!");
	static_assert(MySetting::IsSignature<SignatureB>(), "SignatureB is not a signature!");
	static_assert(MySetting::IsSignature<SignatureC>(), "SignatureC is not a signature!");

	std::cout << "Components: " << MySetting::ComponentCount() << "\n";
	std::cout << "Tags: " << MySetting::TagCount() << "\n";
	std::cout << "Signatures: " << MySetting::SignatureCount() << "\n";

	std::cout << "Component ID of A: " << MySetting::ComponentID<A>() << "\n";
	std::cout << "Component ID of B: " << MySetting::ComponentID<B>() << "\n";
	std::cout << "Component ID of C: " << MySetting::ComponentID<C>() << "\n";
	//std::cout << "Component ID of D: " << MySettings::ComponentID<D>() << "\n";

	std::cout << "Tag ID of TagA: " << MySetting::TagID<TagA>() << "\n";
std::cout << "Tag ID of TagB: " << MySetting::TagID<TagB>() << "\n";
//std::cout << "Tag ID of C: " << MySettings::TagID<C>() << "\n";

std::cout << "Signature ID of SignatureA: " << MySetting::SignatureID<SignatureA>() << "\n";
std::cout << "Signature ID of SignatureB: " << MySetting::SignatureID<SignatureB>() << "\n";
std::cout << "Signature ID of SignatureC: " << MySetting::SignatureID<SignatureC>() << "\n";

std::cout << "RegistryTest All Passed!\n";
}

template <typename TComponent>
using MyFilter = std::integral_constant<bool, MySetting::template IsComponent<TComponent>()>;

template <typename TTag>
using MyFilterT = std::integral_constant<bool, MySetting::template IsTag<TTag>()>;

#define IF_TEST(X, TYPE) if (X) std::cout << "Contain " << #TYPE << "!\n"; else std::cout << "Not Contain " << #TYPE << "!\n"

void TestFilter()
{
	using filter_t = typename TMP::Filter<MyFilterT, TMP::TypeList<A, TagB, B, TagA>>::type;
	IF_TEST(filter_t::Contain<A>(), A);
	IF_TEST(filter_t::Contain<TagA>(), TagA);
	IF_TEST(filter_t::Contain<B>(), B);
	IF_TEST(filter_t::Contain<TagB>(), TagB);
}

void TestApply()
{
	// current bitset A, B, C, TagA, TagB
	// test signature1

	std::array<bool, 5> test;

	for (int i = 0; i < 5; ++i)
		test[i] = false;

	TMP::Apply<TMP::TypeList<TagA>>([&test](auto... t)
		{
			using _dummy = int[];
			(void)_dummy {
				0, (test[MySetting::template TagBitIndex<decltype(t)>()] = true, 0)...
			};
		});

	TMP::Apply<TMP::TypeList<A, B>>([&test](auto... t)
		{
			using _dummy = int[];
			(void)_dummy {
				0, (test[MySetting::template ComponentBitIndex<decltype(t)>()] = true, 0)...
			};
		});

	for (int i = 0; i < 5; ++i)
	{
		if (test[i])
			std::cout << "True!\n";
		else
			std::cout << "False!\n";
	}
}

void TestBitSetsStorage()
{
	TipECS::Impl::SignatureBitSetsStorage<MySetting> storage;
	auto& bitsetA = storage.GetSignatureBitSet<SignatureA>();
	auto& bitsetB = storage.GetSignatureBitSet<SignatureB>();
	auto& bitsetC = storage.GetSignatureBitSet<SignatureC>();

	std::cout << bitsetA << std::endl;
	std::cout << bitsetB << std::endl;
	std::cout << bitsetC << std::endl;
}

void OnComponentAAdded(const TipECS::Entity<MySetting>& entity, A& comp)
{
	std::cout << "A had been added!\n";
}

void OnComponentBAdded(const TipECS::Entity<MySetting>& entity, B& comp)
{
	std::cout << "B had been added!\n";
}

void OnComponentCAdded(const TipECS::Entity<MySetting>& entity, C& comp)
{
	std::cout << "C had been added!\n";
}

void OnComponentARemoved(const TipECS::Entity<MySetting>& entity, A& comp)
{
	std::cout << "A had been removed!\n";
}

void OnComponentBRemoved(const TipECS::Entity<MySetting>& entity, B& comp)
{
	std::cout << "B had been removed!\n";
}

void OnComponentCRemoved(const TipECS::Entity<MySetting>& entity, C& comp)
{
	std::cout << "C had been removed!\n";
}

void OnTagAAdded(const TipECS::Entity<MySetting>& entity)
{
	std::cout << "Tag A had been added!\n";
}

void TestEntityManager()
{
	TipECS::EntityManager<MySetting> entityManager;
	std::vector<TipECS::Entity<MySetting>> myEntities;

	entityManager.GetComponentHooker<A>().HookOnAdded(OnComponentAAdded);
	entityManager.GetComponentHooker<B>().HookOnAdded(OnComponentBAdded);
	entityManager.GetComponentHooker<C>().HookOnAdded(OnComponentCAdded);
	entityManager.GetComponentHooker<A>().HookOnRemoved(OnComponentARemoved);
	entityManager.GetComponentHooker<B>().HookOnRemoved(OnComponentBRemoved);
	entityManager.GetComponentHooker<C>().HookOnRemoved(OnComponentCRemoved);

	entityManager.GetTagHooker<TagA>().HookOnAdded(OnTagAAdded);

	myEntities.push_back(entityManager.CreateEntity());
	myEntities.back().AddComponent<A>(0);
	myEntities.back().AddComponent<C>("Hello!");

	auto [a, c] = entityManager.GetComponent<A, C>(myEntities[0]);
	a.a = 66666;
	c.text = "Test!?";

	myEntities.push_back(entityManager.CreateEntity());
	entityManager.AddComponent<A>(myEntities.back(), 1);
	entityManager.AddComponent<C>(myEntities.back(), "Bye!");

	myEntities.push_back(entityManager.CreateEntity());
	entityManager.AddComponent<A>(myEntities.back(), 2);
	entityManager.AddComponent<C>(myEntities.back(), "No!");

	myEntities.push_back(entityManager.CreateEntity());
	entityManager.AddComponent<A>(myEntities.back(), 3);
	entityManager.AddComponent<C>(myEntities.back(), "Ahhhhhh!");

	myEntities.push_back(entityManager.CreateEntity());
	entityManager.AddComponent<A>(myEntities.back(), 4);

	entityManager.PrintStatus();
	entityManager.DestroyEntity(myEntities[1]);
	entityManager.DestroyEntity(myEntities[3]);
	entityManager.PrintStatus();

	//entityManager.ReFresh();
	//std::cout << "ReFreshed!\n";
	//entityManager.PrintStatus();

	//for (auto& entity : myEntities)
	//{
	//	if (entityManager.IsEntityValid(entity) && entityManager.IsAlive(entity))
	//	{
	//		std::cout << "ID " << entityManager.GetComponent<A>(entity).a << " " <<
	//			"Text: " << entityManager.GetComponent<C>(entity).text << std::endl;
	//	}
	//}
	//std::cout << "\n";

	//entityManager.TraverseEntity([&](const auto& entity)
	//	{
	//		std::cout << "ID " << entityManager.GetComponent<A>(entity).a << " " <<
	//			"Text: " << entityManager.GetComponent<C>(entity).text << std::endl;
	//	});
	//std::cout << "\n";

	//entityManager.TraverseEntityMatchSignature<SignatureC>([](const A& a, const C& c)
	//	{
	//		std::cout << "ID " << a.a << " " <<
	//			"Text: " << c.text << std::endl;
	//	});
	//std::cout << "\n";

	auto view = entityManager.View<A, C>();
	for (auto entity : view)
	{
		auto [a, c] = entity.GetComponent<A, C>();
		std::cout << "ID " << a.a << " " <<
			"Text: " << c.text << std::endl;
		//std::cout << "ID " << entityManager.GetComponent<A>(entity).a << " " <<
		//	"Text: " << entityManager.GetComponent<C>(entity).text << std::endl;
	}
	std::cout << "\n";

	myEntities[1] = entityManager.CreateEntity();
	entityManager.AddComponent<A>(myEntities[1], 1);
	entityManager.AddComponent<C>(myEntities[1], "Bye!");

	myEntities[3] = entityManager.CreateEntity();
	entityManager.AddComponent<A>(myEntities[3], 3);
	entityManager.AddComponent<C>(myEntities[3], "Ahhhhhh!");

	myEntities[1].RemoveComponent<C>();

	entityManager.PrintStatus();

	//for (auto& entity : myEntities)
	//{
	//	if (entityManager.IsEntityValid(entity) && entityManager.IsAlive(entity))
	//	{
	//		std::cout << "ID " << entityManager.GetComponent<A>(entity).a << " " <<
	//			"Text: " << entityManager.GetComponent<C>(entity).text << std::endl;
	//	}
	//}
	//std::cout << "\n";
	
	//entityManager.TraverseEntity([&](const auto& entity)
	//	{
	//		std::cout << "ID " << entityManager.GetComponent<A>(entity).a << " " <<
	//			"Text: " << entityManager.GetComponent<C>(entity).text << std::endl;
	//	});
	//std::cout << "\n";

	entityManager.TraverseEntityMatchSignature<SignatureC>([](const A& a, const C& c)
		{
			std::cout << "ID " << a.a << " " <<
				"Text: " << c.text << std::endl;
		});
	std::cout << "\n";

	entityManager.ReFresh();
	std::cout << "ReFreshed!\n";
	entityManager.PrintStatus();

	auto entity1 = entityManager.CreateEntity();
	auto entity2 = entityManager.CreateEntity();
	auto entity3 = entityManager.CreateEntity();
	entityManager.DestroyEntity(entity2);

	entityManager.PrintStatus();

	entityManager.TraverseEntityMatchSignature<SignatureC>([](const A& a, const C& c)
		{
			std::cout << "ID " << a.a << " " <<
				"Text: " << c.text << std::endl;
		});
	std::cout << "\n";

	entityManager.ReFresh();
	std::cout << "ReFreshed!\n";
	entityManager.PrintStatus();
}

void TestComponentsStorage()
{
	//TipECS::ComponentsStorage<MySettings>::ComponentsType t;
	//size_t n = 100;
	//
	//std::apply([n](auto&... v) 
	//	{
	//		using _dummy = int[];
	//		(void)_dummy { 0, (v.resize(n), 0)... };
	//	}, t);

	//std::apply([n](auto&... v) 
	//{
	//	using _dummy = int[];
	//	(void)_dummy { 0, (std::cout << v.size() << std::endl, 0)... };
	//}, t);
}

void TestAddComponent()
{
}

int main()
{
	//TestTMPLib();
	//TestECSRegistry();
	//TestFilter();
	//TestApply();
	//TestBitSetsStorage();
	TestEntityManager();
	//TestComponentsStorage();
	//TestAddComponent();
}