- [Preference Relation](#preference-relation)
- [Utility function](#utility-function)
- [The rationality assumptions](#the-rationality-assumptions)
- [Extensive form game](#extensive-form-game)
  - [perfect information](#perfect-information)
    - [backward induction](#backward-induction)
    - [von Neumann theorem](#von-neumann-theorem)
  - [different types of solutions](#different-types-of-solutions)
  - [combinatorial games](#combinatorial-games)
  - [nim-sum](#nim-sum)
    - [group](#group)
  - [Strategies](#strategies)
  - [information set](#information-set)
- [Zero sum game](#zero-sum-game)
  - [conservative values](#conservative-values)
  - [例题](#例题)
- [Nash model](#nash-model)
  - [non-cooperative game](#non-cooperative-game)
  - [nash equilibrium](#nash-equilibrium)
    - [BR](#br)
    - [example](#example)
  - [protential](#protential)
    - [判断是否protential](#判断是否protential)
    - [得到protential](#得到protential)
  - [例题](#例题-1)
- [Repeated games - Correlated equilibria](#repeated-games---correlated-equilibria)
  - [例题](#例题-2)
- [cooperative game](#cooperative-game)
  - [TU cooperative game](#tu-cooperative-game)
  - [additive and superadditive](#additive-and-superadditive)
  - [solutions of cooperative games](#solutions-of-cooperative-games)
  - [imputation](#imputation)
  - [core](#core)
  - [nucleolus](#nucleolus)
    - [excess (不满度)](#excess-不满度)
  - [shapley value](#shapley-value)

# Preference Relation  

Let X be a set（所有可能的选择（结果，方案，行动））.   
A perference relation on X is a binary relation $\succeq$（弱偏好（至少不差于））such that for all x,y,z $\in$ X:
- x $\succeq$ x (reflexive)
- x $\succeq$ y or y $\succeq$ x or both (complete)
- If x $\succeq$ y and y $\succeq$ z, then x $\succeq$ z (transitive)

# Utility function

![](picture/image_2.png)

效用值大小本身没有意义，只有比较大小才有意义

当X是有限集合时，效用函数一定存在

![](picture/image_3.png)

# The rationality assumptions

- The players are able to rank the outcomes of the game
- The players are able to provide a utility function for their ranking
- The players use the expected value to build their utility function in presence of random events
- The players are able to analyse all the consequences of their actions, and the consequences of the consequences and so on
- The players use the tools of decision theory whenever possible

# Extensive form game

## perfect information

Each player has knowledge about all the events that previously ocurred

![](picture/image_5.png)

![](picture/image_6.png)

### backward induction

![](picture/image_7.png)

从底往上推：  
对于玩家3，h优于g，j优于k，l优于m，o优于n，所以玩家3的选择为h，j，l，o
对于玩家2，c = (3, 3, 4)优于d = (3, 4, 3), e = (4, 3, 3)优于f = (2, 2, 2)，所以玩家2选择c, e
对于玩家1，b = (4, 3, 3)优于a = (3, 3, 4), 所以玩家1选择a

The rational outcomes of a finite, perfect information game are those given by the procedure of backward induction

### von Neumann theorem

有且仅有一条成立
In the game of chess, one and only one of the following alternatives holds:
- The white has a way to win, no matter what the black does
- The black has a way to win, no matter what the write does
- The white has a way to force at least a draw, no matter what the black does, and the same holds for the black

![](picture/image_8.png)

由冯诺依曼定理可以推导出：  
对于一个finite pefect information game with two players，如果最后的结果只能为某一方获胜的话，则某一方有必胜的方法

## different types of solutions

1. **Very weak solution**: The game has a rational outcome, but it is inaccessible.
2. **Weak solution**: The outcome of the game is know, but how to get to it is not
3. **Solution**: It is possible to provide an algorithm to find a solution.

## combinatorial games

![](picture/image_9.png)

- P-positions(previous player has a winning strategy): loosing
- N-positions(next player has a winning strategy): winning

这里的previous和next是针对当前局面的，而不是某个玩家的（意思是在现在这个局面下，下一个或上一个玩家）

- 最终位置是P-position
- 局面（the state of the game）从P-position只能发展到N-position
- 局面从N-position可以但不必须走到P-position

所以从N-position开始的玩家必胜

## nim-sum

![](picture/image_10.png)

二进制相加但不进位

### group

![](picture/image_11.png)

![](picture/image_12.png)

--- 

![](picture/image_13.png)

Nim-sum为0的位置为P-position

- Terminal position(0, 0, ..., 0)是P-position
- 从任意一个Nim-sum等于0的位置减去任意一个小于$n_i$的数都会使Nim-sum不为0
- Nim-sum不等于0的位置可以通过减去一个数使整体的Nim-sum等于0
  - 找到Nim-sum中最左边的1，在这一位上，一定存在一个pile的该位也是1
  - 选中这一个pile并通过减去一个小于他的数来修改这个pile
    - let z = Nim-sum
    - 把这个pile的最高位从1改成0
    - 之后对于z后面的每一位，如果是0保持不变, 如果是1就通过修改选定的pile使得z的该位翻转为0

因此，对于Nim game，如果初始状态的Nim sum = 0，则说明起始位置为P-position，后手必胜，不为0则是N-position，先手必胜

## Strategies

在strategic form中，strategies不是一次实际走到的路径，而是一个完整的应对计划

![](picture/image_14.png)

左边为extensive form，右边为strategic form

右边的strategic form的意思是，如果P1采用策略s1，而P2采用策略s2，最终的结果是什么

![](picture/image_15.png)

## information set

![](picture/image_16.png)

信息集是玩家在行动时无法区分的一组决策节点  
知道自己在这组节点中的某一个，但不知道具体是哪一个
- $U_i$: 一组玩家i无法区分的节点
- $A_i(U_i)$：在这些节点上，玩家i可以选择的行动

![](picture/image_17.png)

# Zero sum game

![](picture/image_18.png)

## conservative values

![](picture/image_19.png)

玩家一选择一行之后，玩家二会选择这一行中最小的p  
所以对于玩家一来说对于每一行最小的p，选择最大的那一行，$v_1$表示玩家一最少能获得的  

对于玩家二，会找到某一列，这一列中最大的p值在所有列中最小  
此时这个值就是$v_2$，表示玩家二最大需要支付的

![](picture/image_20.png)

![](picture/image_21.png)

![](picture/image_22.png)

## 例题

![](picture/image_23.png)

对于第二问，存在pure strategies意味着$v_1 = v_2$

对于第三问，先观察矩阵通过dominate来删除行或者列，直到对于某一个玩家（或者两者）而言只剩下两种决策  
对于两个玩家而言，不论自己选择哪个策略，对方的混合策略的utility应该要是相等的

# Nash model

## non-cooperative game

![](picture/image_24.png)

## nash equilibrium

![](picture/image_25.png)

- 对于玩家一而言，针对玩家二的选择$\bar{y}$，自己无论怎么选$x$都不会超过$\bar{x}$的效用
  - $f(\bar{x}, \bar{y}) \geq f(x, \bar{y})\ for\ all\ x\in X$
- 对于玩家二而言, 针对玩家一的选择$\bar{x}$, 自己无论怎么选择$y$都不会超过$\bar{y}$的效用
  - $g(\bar{x}, \bar{y}) \geq g(\bar{x}, y)\ for\ all\ y\in Y$

当需要证明纳什平衡时只需要证明这两条

![](picture/image_26.png)

对于这种矩阵，找纳什平衡的方式是针对每一个元素，观察玩家一在这一行上是否utility最大，玩家二在这一列上utility最大  
**注：**纳什平衡不一定只有一个，可能有多个，因此每一个元素都要检查

![](picture/image_27.png)

对于多个玩家的情况，当其他所有玩家都选择纳什平衡策略的情况下，自己无论怎么选最大效用也不会超过选择自己的纳什平衡策略

![](picture/image_28.png)

![](picture/image_29.png)

weakly dominant和strictly dominant的区别就是能不能取等

![](picture/image_30.png)

### BR

![](picture/image_31.png)

对于玩家一，意思是给定玩家2选择y，玩家一所有能使收益最大化的策略集合

### example

![](picture/image_32.png)

![](picture/image_33.png)

计算两个玩家的mixed strategies下的utility，然后根据p,q的值来得到两个BR的分段函数，函数焦点就是纳什平衡

## protential

### 判断是否protential

一个博弈是 exact potential game，当且仅当存在一个函数  
使得任一玩家单边改变策略时  
该玩家效用的变化等于这个函数的变化

判断方法：  
对任意两行$i,i^{'}$, 任意两列$j,j^{'}$检查：  
- 玩家1：  
  - $u_1(i,j)-u_1(i^{'},j)-(u_1(i,j^{'})-u_1(i^{'},j^{'}))$
  - 固定某一列不同两行相减，相减的结果在相减
- 玩家2：
  - $u_2(i,j)-u_2(i,j^{'})-(u_2(i^{'},j)-u_2(i^{'},j^{'}))$
  - 固定某一行不同两列相减  

最终这两个值要相等

### 得到protential

选定（1，1）为基准点0  
固定一行从左往右移动列看玩家1的utility的变化，对应的protential从左往右加上这个变化的值  
从上往下同理  
如果这个game是protential的，那么不同的路径不会影响protential的值

## 例题

![](picture/image_34.png)

- 对于第一二题这种给定一个玩家（A）的混合策略而不知道另一个玩家（B）的混合策略的题目，主要是两点：
  - A的混合策略中不为0的策略，说明在B的混合策略下，得到的utility是相等的（可以得到若干个等式）
  - 如果A的混合策略中有为0的，说明其他概率不为0的策略在B的混合策略下均优于这个策略（可以得到若干个不等式）
- 对于第三题这种给定一个玩家（A）在纳什平衡下的混合策略要你求另一个（B）的：
  - 在纳什平衡下，对于你自己策略的每一条，在对手的混合策略下算出的utility都相等
  - 根据对手已有的混合策略，可以通过计算自己每个纯策略的期望来排除某个策略

**注：**这些题目都可能会穿插使用dominate来排除一些行和列

# Repeated games - Correlated equilibria

![](picture/image_35.png)

## 例题

![](picture/image_36.png)

# cooperative game

玩家可以组成联盟，并且关心联盟能创造多少价值，以及这个价值如何在联盟成员之间分配

## TU cooperative game

一个TU合作博弈由： （N, v）组成  
其中： 
- $N=\{1,2,...,n\}$ : 玩家集合
- $v:2^N->R$

The fact that the utility v (also called side-payment function) takes on real values
reflects the idea that the aggregated utility v(A) can be freely divided among the
members of the coalition A.

函数$v(A)$表示联盟$A$能保证获得的最大收益

Example:  
![](picture/image_37.png)

Any TU game is also a cooperative game: in fact, the value v(A) can be replaced
by a subset V(A) 

$V(A) = \{ (x_i)_{i \in A} : \sum_{i \in A} x_i \le v(A) \}$

$x_i$：分给玩家i的效用  
联盟总分配不能超过联盟创造的价值

## additive and superadditive

![](picture/image_39.png)

**注：** A和B两个集合不能相交

## solutions of cooperative games

![](picture/image_40.png)

## imputation

imputation 和 core 都是给“所有玩家𝑁”分配收益的向量

![](picture/image_41.png)

imputation试一种分钱方案，规定每个玩家最后能拿多少钱

条件一说明玩家i至少要拿到他单干时能保证的收益

条件二表示所有人分的钱总和要等于整个联盟的收益

## core

![](picture/image_42.png)

条件一：联盟的总价值被完整分配

条件二：任何联盟S拿到的合计分成都大于等于这个联盟自己单走合作能创造的价值

Example:  
![](picture/image_43.png)

## nucleolus

用于在core可能有很多甚至为空时，挑选一个最公平最稳定的分配  
让最不满意的联盟尽可能不那么不满意  

### excess (不满度)

![](picture/image_44.png)

nucleolus 是使 “excess 向量” 按字典序最小的分配

## shapley value

每个玩家对整体合作平均贡献了多少，就该分到多少

设想玩家一个一个随机加入联盟，看他加入时带来多少边际增益  
对所有的顺序的结果相加再取平均

边际贡献：  
对于联盟$S$
$$
\Delta_i(S) = v(S \cup \{i\}) - v(S)
$$
