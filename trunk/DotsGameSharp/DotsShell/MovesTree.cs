using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace DotsShell
{
	/// <remarks>
	/// Type of label
	/// </remarks>
	public enum LabelType
	{
		Triangle,
		Square,
		Circle,
	}

	/// <remarks>
	/// Label for marking any positions on field
	/// </remarks>
	public class Label
	{
		[XmlAttribute]
		public LabelType Type;
		[XmlAttribute]
		public int X { get; set; }
		[XmlAttribute]
		public int Y { get; set; }

		public Label(int X, int Y, LabelType Type)
		{
			this.X = X;
			this.Y = Y;
			this.Type = Type;
		}
	}

	/// <remarks>
	/// Element of MovesTree. It is describe one move with/without labels
	/// </remarks>
	public class MovesTreeNode
	{
		[XmlAttribute]
		public readonly int X;
		[XmlAttribute]
		public readonly int Y;

		public List<Label> Labels;

		public readonly MovesTreeNode Parent;
		List<MovesTreeNode> Childrens;

		private MovesTreeNode(MovesTreeNode ParentNode, int X, int Y, List<Label> Labels = null)
		{
			this.Parent = ParentNode;
			this.X = X;
			this.Y = Y;
			if (Labels != null && Labels.Count == 0)
				Labels = null;
			else
				this.Labels = Labels;

			//if (ParentNode != null)
			//    ParentNode.Childrens.Add(this);
		}

		public MovesTreeNode(int X, int Y, List<Label> Labels = null)
		{
			this.Parent = null;
			this.X = X;
			this.Y = Y;
			if (Labels != null && Labels.Count == 0)
				Labels = null;
			else
				this.Labels = Labels;
		}

		public void AddChildren(int X, int Y, List<Label> Labels = null)
		{
			if (Childrens == null)
				Childrens = new List<MovesTreeNode>(1);
			Childrens.Add(new MovesTreeNode(this, X, Y, Labels));
		}

		public MovesTreeNode GetChildren(int Index)
		{
			return Childrens[Index];
		}

		/*
		private void RemoveChildrens()
		{
			if (Childrens != null)
				foreach (MovesTreeNode Node in Childrens)
					Node.RemoveChildrens();
		}

		public void RemoveChildren(int Index)
		{
			foreach (MovesTreeNode Node in Childrens) ;
		}*/
	}

	public delegate void MovesTreeHandler(object sender, EventArgs e);

	/// <remarks>
	/// Tree of all moves. Consists of MovesTreeNode
	/// </remarks>
	public class MovesTree
	{
		public MovesTreeHandler Draw;

		protected void OnDraw(EventArgs e)
		{
			if (Draw != null)
				Draw(this, e);
		}

		List<MovesTreeNode> Roots;
		MovesTreeNode CurNode;

		public MovesTree()
		{
			Roots = new List<MovesTreeNode>(1);
		}

		public void Add(int X, int Y)
		{
			if (Roots.Count == 0)
			{
				Roots.Add(new MovesTreeNode(X, Y));
				CurNode = Roots[0];
			}
			else
			{
				CurNode.AddChildren(X, Y);
				CurNode = CurNode.GetChildren(0);
			}
		}
	}
}
