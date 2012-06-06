forced_attack(Dot player)
{
	var enemy_player = player.NextPlayer();
	MovesSequence forced_attack_sqs;
	int first_enemy_pos;
	
	int best_move = -1;
	float score = 0;
	int depth = 0;
	
	forced_attack_sqs = find_n_captures_moves(player, 2);
	while (forced_attack_sqs.Count != 0)
	{
		first_enemy_pos = forced_attack_sqs.SurroundedPositions[0];
		foreach (var sequence in forced_attack_sqs)
		{
			foreach (var sq_order in sequence.get_orders())
			{
				Field.make_move(sq_order.CapturePositions[0], player);
				depth++;
				
				var forced_enemy_attack_sqs = find_captures_moves(enemy_player); // find_n_captures_moves(enemy_player, 1);
				
				if (forced_enemy_attack_sqs.Surround(sequence.ChainPositions) ||
					forced_enemy_attack_sqs.Surround(sq_order.CapturePositions[0]))
						break;
				
				Field.make_move(sq_order.CapturePositions[1], enemy_player);
				depth++;
			}
			
			var forced_attack_sqs_1 = find_captures_moves(player);
			if (forced_attack_sqs_1.Count != 0 &&
				forced_attack_sqs_1.SurroundedPositions.Contains(first_enemy_pos))
				{
					best_move = 
				}
		}
		
		forced_attack_sqs = find_n_captures_moves(player, 2);
	}
	
	Field.UnmakeMoves(depth);
}

MovesSequence find_n_captures_moves(Dot player, int n)
{
}

MovesSequence find_captures_moves(Dot player)
{
	return find_n_captures_moves(player, 1);
}

MovesSequence find_defence_moves(Dot player)
{
	return find_n_captures_moves(player.NextPlayer(), 1);
}

class MovesSequence
{
	List<int> ChainPositions;
	List<int> SurroundedPositions;
	
	List<int> CapturePositions;
	
	int[][] get_orders();
}