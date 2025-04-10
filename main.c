#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// Estados combinados PORTD
#define VAI_CARRO           0b01000110  // Verde carro (PD6), Vermelho pedestre (PD1)
#define ATENCAO_CARRO       0b10000110  // Amarelo carro (PD7), Vermelho pedestre (PD1)
#define VAI_PED             0b00101100  // Verde pedestre (PD2), Vermelho carro (PD5)
#define PISCA_PED_VM        0b00100000  // Somente verde pedestre (PD2)
#define PISCA_PED           0b00000000  // Tudo apagado

int main(void)
{
    // Configura botão como entrada (PD3) com pull-up
    DDRD &= ~(1 << PD3);
    PORTD |= (1 << PD3);

    // Configura os LEDs como saída (PD1, PD2, PD5, PD6, PD7)
    DDRD |= (1 << PD1) | (1 << PD2) | (1 << PD5) | (1 << PD6) | (1 << PD7);

    int estado = 0;
    int tempo = 0;

    while (1)
    {
        if (estado == 0) // Pedestre atravessa
        {
            PORTD = VAI_PED | (1 << PD3);
            tempo = 15000;
            estado = 1;
        }

        if (estado == 1) // Contagem e pisca do pedestre
        {
            --tempo;
            _delay_ms(1);

            if (tempo == 5000) PORTD = PISCA_PED_VM | (1 << PD3);
            if (tempo == 4000) PORTD = PISCA_PED     | (1 << PD3);
            if (tempo == 3000) PORTD = PISCA_PED_VM | (1 << PD3);
            if (tempo == 2000) PORTD = PISCA_PED     | (1 << PD3);
            if (tempo == 1000) PORTD = PISCA_PED_VM | (1 << PD3);

            if (tempo == 0) estado = 2;
        }

        if (estado == 2) // Verde para veículos
        {
            PORTD = VAI_CARRO | (1 << PD3);
            tempo = 42000;
            estado = 3;
        }

        if (estado == 3) // Contagem do verde veículo + verificação botão
        {
            --tempo;
            _delay_ms(1);

            if (!(PIND & (1 << PD3))) // Botão pressionado (nível baixo)
            {
                if (tempo > 10000) // Antecipar estado se faltar mais de 10s
                {
                    estado = 4;
                }
            }

            if (tempo == 0) estado = 4;
        }

        if (estado == 4) // Amarelo para veículos
        {
            PORTD = ATENCAO_CARRO | (1 << PD3);
            tempo = 3000;
            estado = 5;
        }

        if (estado == 5) // Aguarda fim do amarelo
        {
            --tempo;
            _delay_ms(1);
            if (tempo == 0) estado = 0;
        }
    }
}




