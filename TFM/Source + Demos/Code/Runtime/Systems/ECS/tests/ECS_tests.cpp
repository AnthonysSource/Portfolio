#include "CookieKat/Systems/ECS/EntityDatabase.h"
#include <gtest/gtest.h>

using namespace CKE;

struct DataComp1
{
	u32 a;
	u16 b;
	u16 c;
	u64 d;

	static DataComp1 DefaultValues() { return DataComp1{0, 1, 2, 3}; }

	friend bool operator==(const DataComp1& lhs, const DataComp1& rhs) {
		return lhs.a == rhs.a
				&& lhs.b == rhs.b
				&& lhs.c == rhs.c
				&& lhs.d == rhs.d;
	}

	friend bool operator!=(const DataComp1& lhs, const DataComp1& rhs) { return !(lhs == rhs); }
};

struct DataComp2
{
	i8  a;
	i32 b;
	i64 c;
	i16 d;

	static DataComp2 DefaultValues() { return DataComp2{ 0, 1, 2, 3 }; }

	friend bool operator==(const DataComp2& lhs, const DataComp2& rhs) {
		return lhs.a == rhs.a
				&& lhs.b == rhs.b
				&& lhs.c == rhs.c
				&& lhs.d == rhs.d;
	}

	friend bool operator!=(const DataComp2& lhs, const DataComp2& rhs) { return !(lhs == rhs); }
};

struct Comp2
{
	i32 a = -53;
};

struct Comp3
{
	f64 a = 3.141516;
};

struct Comp4
{
	u8 a = 255;
};

class EntityDatabaseTest : public testing::Test
{
protected:
	void SetUp() override {
		m_EntityDB = EntityDatabase{};
		m_EntityDB.Initialize(MAX_ENTITIES);
		m_Debugger = m_EntityDB.GetDebugger();
		m_EntityDB.RegisterComponent<DataComp1>();
		m_EntityDB.RegisterComponent<DataComp2>();

		m_EntityDB.RegisterComponent<Comp2>();
		m_EntityDB.RegisterComponent<Comp3>();
		m_EntityDB.RegisterComponent<Comp4>();
	}

	void TearDown() override {
		m_EntityDB.Shutdown();
	}

	static constexpr u32   MAX_ENTITIES = 100;
	EntityDatabase         m_EntityDB{};
	EntityDatabaseDebugger m_Debugger{};
};

TEST_F(EntityDatabaseTest, AddRemEntity_Simple) {
	EXPECT_EQ(m_Debugger.GetStateSnapshot().m_NumEntities, 0);
	EntityID e1 = m_EntityDB.CreateEntity();
	EXPECT_EQ(m_Debugger.GetStateSnapshot().m_NumEntities, 1);
	EntityID e2 = m_EntityDB.CreateEntity();
	EXPECT_EQ(m_Debugger.GetStateSnapshot().m_NumEntities, 2);
	m_EntityDB.DeleteEntity(e1);
	EXPECT_EQ(m_Debugger.GetStateSnapshot().m_NumEntities, 1);
	m_EntityDB.DeleteEntity(e2);
	EXPECT_EQ(m_Debugger.GetStateSnapshot().m_NumEntities, 0);
}

TEST_F(EntityDatabaseTest, AddRemove_MaxEntities) {
	EXPECT_EQ(m_Debugger.GetStateSnapshot().m_NumEntities, 0);

	EntityID ids[MAX_ENTITIES];
	for (int i = 0; i < MAX_ENTITIES; ++i) {
		ids[i] = m_EntityDB.CreateEntity();
	}

	EXPECT_EQ(m_Debugger.GetStateSnapshot().m_NumEntities, MAX_ENTITIES);

	for (int i = 0; i < MAX_ENTITIES; ++i) {
		m_EntityDB.DeleteEntity(ids[i]);
	}

	EXPECT_EQ(m_Debugger.GetStateSnapshot().m_NumEntities, 0);
}

TEST_F(EntityDatabaseTest, AddingTooManyEntitiesOverflows) {
	EXPECT_EQ(m_Debugger.GetStateSnapshot().m_NumEntities, 0);
	for (int i = 0; i < MAX_ENTITIES; ++i) {
		m_EntityDB.CreateEntity();
	}
	EXPECT_DEATH(m_EntityDB.CreateEntity(), "Assertion failed");
}

TEST_F(EntityDatabaseTest, AddUnregisteredComponentFails) {
	EntityID e = m_EntityDB.CreateEntity();
	struct UnregisteredComp1
	{
		i32 a;
	};
	EXPECT_DEATH(m_EntityDB.AddComponent<UnregisteredComp1>(e, UnregisteredComp1{0}), "Assertion failed");
}

TEST_F(EntityDatabaseTest, General) {
	EXPECT_EQ(m_Debugger.GetStateSnapshot().m_NumEntities, 0);

	EntityID e1[13];
	EntityID e2[15];
	EntityID e3[7];

	for (EntityID& id : e1) {
		id = m_EntityDB.CreateEntity();
		m_EntityDB.AddComponent<Comp2>(id);
	}

	for (EntityID& id : e2) {
		id = m_EntityDB.CreateEntity();
		m_EntityDB.AddComponent<Comp3>(id);
		m_EntityDB.AddComponent<Comp2>(id);
	}

	for (EntityID& id : e3) {
		id = m_EntityDB.CreateEntity();
		m_EntityDB.AddComponent<Comp4>(id);
		m_EntityDB.AddComponent<Comp2>(id);
		m_EntityDB.AddComponent<Comp3>(id);
	}

	for (EntityID& id : e1) {
		EXPECT_EQ(m_EntityDB.GetComponent<Comp2>(id)->a, -53);
	}

	for (EntityID& id : e2) {
		EXPECT_EQ(m_EntityDB.GetComponent<Comp2>(id)->a, -53);
		EXPECT_EQ(m_EntityDB.GetComponent<Comp3>(id)->a, 3.141516);
	}

	for (EntityID& id : e3) {
		EXPECT_EQ(m_EntityDB.GetComponent<Comp2>(id)->a, -53);
		EXPECT_EQ(m_EntityDB.GetComponent<Comp3>(id)->a, 3.141516);
		EXPECT_EQ(m_EntityDB.GetComponent<Comp4>(id)->a, 255);
	}

	EntityDatabaseStateSnapshot snapshot = m_Debugger.GetStateSnapshot();

	EXPECT_EQ(snapshot.m_NumEntities, 35);
	EXPECT_GE(snapshot.m_NumArchetypes, 4);

	m_EntityDB.Shutdown();
}

TEST_F(EntityDatabaseTest, EntityRecordGetsUpdatedWhenRemovingEntity) {
	EntityID  e1 = m_EntityDB.CreateEntity();
	DataComp1 c1{0, 1, 2, 3};
	m_EntityDB.AddComponent<DataComp1>(e1, c1);
	DataComp1* comp1 = m_EntityDB.GetComponent<DataComp1>(e1);
	EXPECT_TRUE(*comp1 == c1);

	EntityID  e2 = m_EntityDB.CreateEntity();
	DataComp1 c2{4, 5, 6, 7};
	m_EntityDB.AddComponent<DataComp1>(e2, c2);
	DataComp1* comp2 = m_EntityDB.GetComponent<DataComp1>(e2);
	EXPECT_TRUE(*comp2 == c2);

	m_EntityDB.DeleteEntity(e1);

	comp2 = m_EntityDB.GetComponent<DataComp1>(e2);
	EXPECT_TRUE(*comp2 == c2);
}

TEST_F(EntityDatabaseTest, AddInterleavedEntities) {
	EntityID  e1 = m_EntityDB.CreateEntity();
	EntityID  e2 = m_EntityDB.CreateEntity();
	DataComp1 c1 = DataComp1{4, 5, 6, 7};
	DataComp1 c2 = DataComp1{0, 1, 2, 3};
	m_EntityDB.AddComponent<DataComp1>(e1, c1);
	m_EntityDB.AddComponent<DataComp1>(e2, c2);

	DataComp1* comp1 = m_EntityDB.GetComponent<DataComp1>(e1);
	EXPECT_TRUE(*comp1 == c1);
	DataComp1* comp2 = m_EntityDB.GetComponent<DataComp1>(e2);
	EXPECT_TRUE(*comp2 == c2);

	m_EntityDB.AddComponent<DataComp2>(e1);
	m_EntityDB.AddComponent<DataComp2>(e2);

	comp1 = m_EntityDB.GetComponent<DataComp1>(e1);
	EXPECT_TRUE(*comp1 == c1);
	comp2 = m_EntityDB.GetComponent<DataComp1>(e2);
	EXPECT_TRUE(*comp2 == c2);
}

TEST_F(EntityDatabaseTest, IterateOneComponent) {
	EntityID  e1 = m_EntityDB.CreateEntity();
	EntityID  e2 = m_EntityDB.CreateEntity();
	DataComp1 c1 = DataComp1{4, 5, 6, 7};
	DataComp1 c2 = DataComp1{0, 1, 2, 3};
	m_EntityDB.AddComponent<DataComp1>(e1, c1);
	m_EntityDB.AddComponent<DataComp1>(e2, c2);

	DataComp1 c{1, 1, 1, 1};
	for (DataComp1* pComp : m_EntityDB.GetSingleCompIter<DataComp1>()) {
		*pComp = c;
	}

	for (DataComp1* pComp : m_EntityDB.GetSingleCompIter<DataComp1>()) {
		EXPECT_TRUE(*pComp == c);
	}
}


TEST_F(EntityDatabaseTest, IterateOneComponentT) {
	//Array<EntityID, 100> e{};

	//for (i32 i = 0; i < 50; ++i) {
	//	e[i] = m_EntityDB.CreateEntity();
	//	m_EntityDB.AddComponent<DataComp1>(e[i], DataComp1::DefaultValues());
	//}

	//for (i32 i = 50; i < 100; ++i) {
	//	e[i] = m_EntityDB.CreateEntity();
	//	m_EntityDB.AddComponent<DataComp2>(e[i], DataComp2::DefaultValues());
	//	m_EntityDB.AddComponent<DataComp1>(e[i], DataComp1::DefaultValues());
	//}

	//i32 i = 0;
	//for (auto&& [d1, d2] : m_EntityDB.GetMultiCompTupleIter<DataComp1, DataComp2>()) {
	//	d1->a = i;
	//	d1->b = i * 2;
	//	d1->c = i * 3;
	//	d1->d = i * 4;
	//}
}