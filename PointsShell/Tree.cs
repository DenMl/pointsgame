using System.Collections.Generic;

namespace PointsShell
{
	class Tree<T>
	{
		public T Data { get; set; }
		public Tree<T> Parent { get; protected set; }
		public List<Tree<T>> Children { get; protected set; }

		public void AddChild(Tree<T> child)
		{
			if (child.Parent != null)
				child.Parent.RemoveChild(child);
			child.Parent = this;
			Children.Add(child);
		}

		public void RemoveChild(Tree<T> child)
		{
			if (child.Parent != this)
				return;
			child.Parent = null;
			Children.Remove(child);
		}
	}
}
